#include "Context.h"

#include <utility>
#include <cstring>
#include <iostream>
#include <thread>

Context::Context(const std::string& name, JSRuntime *rt, JNIEnv* env) {
    this->ctx = JS_NewContext(rt);
    this->name = name;
    this->env = env;

    this->run_loop_stopped = true;
    this->end_run_loop = false;

    this->init_api_console();
    this->init_api_event_listeners();
    this->init_api_crypto();
    this->init_api_timeout();
    this->init_api_text();
    this->init_api_kv();

    JS_SetContextOpaque(this->ctx, this);

    JSValue global_obj = JS_GetGlobalObject(this->ctx);
    this->global_json_obj = JS_GetPropertyStr(this->ctx, global_obj, "JSON");
    JS_FreeValue(this->ctx, global_obj);
}

Context::~Context() {
    this->stop_run_loop();

    for(const auto& kv: this->event_listeners) {
        JS_FreeValue(this->ctx, kv.second);
    }

    for(const auto& kv: this->timers) {
        JS_FreeValue(this->ctx, kv.second.js_func);
    }

    JS_FreeValue(this->ctx, this->global_json_obj);
    JS_FreeContext(this->ctx);
}

void Context::start_run_loop() {
    this->end_run_loop = false;

    this->run_loop_thread = std::thread([this] {
       run_loop();
    });

    this->run_loop_thread.detach();

    this->run_loop_stopped = false;
}

void Context::stop_run_loop() {
    if (this->end_run_loop) {
        return;
    }

    this->run_loop_mutex.lock();
    this->end_run_loop = true;
    this->run_loop_mutex.unlock();

    while(!this->run_loop_stopped) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void Context::run_loop()
{
    for(;;) {
        this->run_loop_mutex.lock();
        if (this->end_run_loop) {
            break;
        }
        this->run_loop_mutex.unlock();

        if (!this->timers.empty()) {
            this->timers_mutex.lock();
            JSValue global_obj = JS_GetGlobalObject(this->ctx);

            for(const auto& kv: this->timers) {
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - kv.second.start);

                if (duration.count() >= kv.second.timeout) {
                    this->timers[kv.first].start = std::chrono::system_clock::now();

                    JSValue value = JS_Call(this->ctx, this->timers[kv.first].js_func, global_obj, 0, nullptr);
                    JS_FreeValue(this->ctx, value);

                    if (!this->timers[kv.first].repeat) {
                        JS_FreeValue(this->ctx, this->timers[kv.first].js_func);
                        this->timers.erase(kv.first);
                    }
                }
            }

            this->timers_mutex.unlock();

            JS_FreeValue(this->ctx, global_obj);
        }
    }

    this->run_loop_stopped = true;
}

JSValue Context::parseJSON(const std::string& json_string) const
{
    JSValue ret_value = JS_UNDEFINED;

    JSValue global_obj = JS_GetGlobalObject(this->ctx);
    JSValue parse_func_obj = JS_GetPropertyStr(this->ctx, this->global_json_obj, "parse");

    JSValue json = JS_NewString(this->ctx, json_string.c_str());

    JSValue parsed = JS_Call(this->ctx, parse_func_obj, global_obj, 1, &json);

    // look for __ebr:: and replace with JSFunction
    JSPropertyEnum *properties;
    uint32_t count;
    JS_GetOwnPropertyNames(this->ctx, &properties, &count, parsed, JS_GPN_STRING_MASK|JS_GPN_SYMBOL_MASK|JS_GPN_ENUM_ONLY);
    for (uint32_t i = 0; i < count; i++) {
        JSAtom atom = properties[i].atom;
        const char *key = JS_AtomToCString(ctx, atom);

        JSValue val = JS_GetProperty(ctx, parsed, atom);

        if (JS_IsString(val)) {
            const char *c_str_val = JS_ToCString(ctx, val);
            std::string str_value = std::string(c_str_val);

            if(str_value.starts_with("__ebr::")) {
                try {
                    auto global_func_name = str_value.substr(6);
//                    this->function_index.at(global_func_name);

                    JS_SetPropertyStr(this->ctx, parsed, key, JS_NewCFunction(this->ctx, call_global_function, global_func_name.c_str(), 1));

                } catch(std::exception &ex) {}
            }

            JS_FreeCString(this->ctx, c_str_val);
        }

        JS_FreeValue(this->ctx, val);
        JS_FreeCString(this->ctx, key);
    }

    ret_value = JS_DupValue(this->ctx, parsed);

    JS_FreeValue(this->ctx, parsed);
    JS_FreeValue(this->ctx, json);
    JS_FreeValue(this->ctx, parse_func_obj);
    JS_FreeValue(this->ctx, global_obj);

    return ret_value;
}

std::string Context::stringifyJSON(JSValue object) const
{
    JSValue global_obj = JS_GetGlobalObject(this->ctx);
    JSValue stringify_func_obj = JS_GetPropertyStr(this->ctx, this->global_json_obj, "stringify");

    JSValue str_value = JS_Call(this->ctx, stringify_func_obj, global_obj, 1, &object);

    const char * c_str = JS_ToCString(this->ctx, str_value);

    std::string json(c_str);

    JS_FreeCString(this->ctx, c_str);

    JS_FreeValue(this->ctx, str_value);
    JS_FreeValue(this->ctx, stringify_func_obj);
    JS_FreeValue(this->ctx, global_obj);

    return json;
}

JSValue Context::evaluate(const std::string& code, bool ret_val) const {
    JSValue ret_value = JS_UNDEFINED;

    int flags = JS_EVAL_TYPE_GLOBAL | JS_EVAL_FLAG_BACKTRACE_BARRIER;

    size_t code_len = strlen(code.c_str());

    JSValue value = JS_Eval(this->ctx, code.c_str(), code_len, "<code>", flags);

    if (JS_IsException(value)) {
        ret_value = JS_DupValue(this->ctx, value);
        JS_FreeValue(this->ctx, value);

        return ret_value;
    }

    if (ret_val)  {
        std::string json = this->stringifyJSON(value);

        JSValue json_value = JS_NewString(this->ctx,json.c_str());
        ret_value = JS_DupValue(this->ctx, json_value);

        JS_FreeValue(this->ctx, json_value);
    }

    JS_FreeValue(this->ctx, value);
    return ret_value;
}

JSValue Context::dispatch_event(const std::string &event, JSValue details) {
    JSValue ret_value = JS_UNDEFINED;
    JSValue global_obj = JS_GetGlobalObject(this->ctx);

    auto range = this->event_listeners.equal_range(event);
    for (auto itr = range.first; itr != range.second; ++itr) {
        auto callback = (JSValue)itr->second;

        JSValue value = JS_Call(this->ctx, callback, global_obj, 1, &details);
        if (JS_IsException(value)) {
            ret_value = JS_DupValue(this->ctx, value);
            JS_FreeValue(this->ctx, value);
            break;
        }

        JS_FreeValue(this->ctx, value);
    }

    JS_FreeValue(this->ctx, global_obj);
    return ret_value;
}

void Context::register_function(const std::string& func_name, jobject func) {
    int size = this->functions.size();

    this->function_index[func_name] = size;
    this->functions.push_back(func);

    JSValue global_obj = JS_GetGlobalObject(this->ctx);

    JS_SetPropertyStr(this->ctx, global_obj, func_name.c_str(), JS_NewCFunction(this->ctx, call_global_function, func_name.c_str(), 1));

    JS_FreeValue(this->ctx, global_obj);
}

void Context::init_api_console() const
{
    JSValue global_obj, console;

    global_obj = JS_GetGlobalObject(this->ctx);
    console = JS_NewObject(this->ctx);

    JS_SetPropertyStr(this->ctx, console, "log", JS_NewCFunction(ctx, api_console_log, "log", 1));
    JS_SetPropertyStr(this->ctx, console, "info", JS_NewCFunction(ctx, api_console_log, "info", 1));
    JS_SetPropertyStr(this->ctx, console, "warn", JS_NewCFunction(ctx, api_console_warn, "warn", 1));
    JS_SetPropertyStr(this->ctx, console, "error", JS_NewCFunction(ctx, api_console_error, "error", 1));
    JS_SetPropertyStr(this->ctx, console, "debug", JS_NewCFunction(ctx, api_console_debug, "debug", 1));

    JS_SetPropertyStr(this->ctx, global_obj, "console", console);

    JS_FreeValue(this->ctx, global_obj);
}

void Context::init_api_event_listeners() const
{
    JSValue global_obj = JS_GetGlobalObject(this->ctx);

    JS_SetPropertyStr(this->ctx, global_obj, "addEventListener", JS_NewCFunction(this->ctx, api_add_event_listener, "addEventListener", 2));

    JS_FreeValue(this->ctx, global_obj);
}

void Context::init_api_crypto() const
{
    JSValue global_obj, crypto;

    global_obj = JS_GetGlobalObject(this->ctx);

    crypto = JS_NewObject(this->ctx);

    JS_SetPropertyStr(this->ctx, crypto, "getRandomValues", JS_NewCFunction(ctx, api_crypto_get_random_values, "getRandomValues", 1));
    JS_SetPropertyStr(this->ctx, crypto, "randomUUID", JS_NewCFunction(ctx, api_crypto_random_uuid, "randomUUID", 0));

    JS_SetPropertyStr(this->ctx, global_obj, "crypto", crypto);

    JS_FreeValue(this->ctx, global_obj);
}

void Context::init_api_timeout() const
{
    JSValue global_obj = JS_GetGlobalObject(this->ctx);

    JS_SetPropertyStr(this->ctx, global_obj, "setTimeout", JS_NewCFunction(this->ctx, api_set_timeout, "setTimeout", 2));
    JS_SetPropertyStr(this->ctx, global_obj, "clearTimeout", JS_NewCFunction(this->ctx, api_clear_timeout, "clearTimeout", 1));
    JS_SetPropertyStr(this->ctx, global_obj, "setInterval", JS_NewCFunction(this->ctx, api_set_interval, "setInterval", 1));
    JS_SetPropertyStr(this->ctx, global_obj, "clearInterval", JS_NewCFunction(this->ctx, api_clear_timeout, "clearInterval", 1));

    JS_FreeValue(this->ctx, global_obj);
}

void Context::init_api_text() const
{
    init_text_encoder_class(this->ctx);
    init_text_decoder_class(this->ctx);
}

void Context::init_api_kv() const
{
    JSValue global_obj, kv;

    global_obj = JS_GetGlobalObject(this->ctx);

    kv = JS_NewObject(this->ctx);

    JS_SetPropertyStr(this->ctx, kv, "set", JS_NewCFunction(ctx, api_kv_set, "set", 2));
    JS_SetPropertyStr(this->ctx, kv, "get", JS_NewCFunction(ctx, api_kv_get, "get", 1));
    JS_SetPropertyStr(this->ctx, kv, "remove", JS_NewCFunction(ctx, api_kv_remove, "remove", 1));

    JS_FreeValue(this->ctx, global_obj);
}

std::string get_function_name(JSContext *ctx)
{
    JS_FreeValue(ctx, JS_ThrowTypeError(ctx, "callstack"));
    JSValue exception = JS_GetException(ctx);

    JSValue stack_val = JS_GetPropertyStr(ctx, exception, "stack");
    std::string callstack( JS_ToCString(ctx, stack_val));

    auto first_line = callstack.substr(0, callstack.find("\n"));

    auto name = first_line.replace(first_line.find("(native)"), sizeof("(native)") -1, "");
    name.erase(std::remove_if(name.begin(), name.end(), ::isspace), name.end());
    name.erase(0,2);

    JS_FreeValue(ctx, stack_val);
    JS_FreeValue(ctx, exception);
    JS_ResetUncatchableError(ctx);

    return name;
}

JSValue call_global_function(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    JSValue ret_value = JS_UNDEFINED;

    auto func_name = get_function_name(ctx);

    // TODO: Check for JNI Exceptions
    Context *parent_ctx = (Context *)JS_GetContextOpaque(ctx);

    int func_index = parent_ctx->function_index[func_name];
    auto j_func = parent_ctx->functions[func_index];

    if (j_func != nullptr) {
        jclass j_function_class = parent_ctx->env->GetObjectClass(j_func);
        jmethodID j_method = parent_ctx->env->GetMethodID(j_function_class, "run", "()V");

        JSValue args = argv[0];
        if (!JS_IsNull(args) && !JS_IsUndefined(args)) {
            auto json_string = parent_ctx->stringifyJSON(args);
            jstring j_json_string = parent_ctx->env->NewStringUTF(json_string.c_str());

            // create a JSONObject
            jclass json_object_c = parent_ctx->env->FindClass("org/json/JSONObject");
            jmethodID json_object_cnstrctr = parent_ctx->env->GetMethodID(json_object_c, "<init>", "(Ljava/lang/String;)V");

            jobject json_object = parent_ctx->env->NewObject(json_object_c, json_object_cnstrctr, j_json_string);

            jfieldID args_field = parent_ctx->env->GetFieldID(j_function_class, "args", "Lorg/json/JSONObject;");

            parent_ctx->env->SetObjectField(j_func, args_field, json_object);
        }

        parent_ctx->env->CallVoidMethod(j_func, j_method);
    }

    return ret_value;
}



