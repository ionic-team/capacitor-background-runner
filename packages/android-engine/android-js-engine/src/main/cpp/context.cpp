#include "context.h"
#include "errors.h"
#include <cstring>
#include <iostream>
#include <thread>
#include <utility>

JavaFunctionData::JavaFunctionData(const std::string &name, jobject func) {
    this->name = name;
    this->j_func = func;
}

JavaFunctionData::~JavaFunctionData() {}

Context::Context(const std::string& name, JSRuntime* rt, JNIEnv *env, jobject api_instance) {
    this->ctx = JS_NewContext(rt);
    this->name = name;
    this->env = env;

    this->env->GetJavaVM(&this->vm);

    this->api = api_instance;

    this->run_loop_stopped = true;
    this->end_run_loop = false;

    this->jni_classes = new JNIClasses(env);

    this->init_api_console();
    this->init_api_event_listeners();
    this->init_api_crypto();
    this->init_api_timeout();
    this->init_api_text();
    this->init_api_fetch();

    JS_SetContextOpaque(this->ctx, this);

    JSValue global_obj = JS_GetGlobalObject(this->ctx);
    this->global_json_obj = JS_GetPropertyStr(this->ctx, global_obj, "JSON");
    JS_FreeValue(this->ctx, global_obj);
}

Context::~Context() {
    this->stop_run_loop();

    for (const auto &kv : this->event_listeners) {
        JS_FreeValue(this->ctx, kv.second);
    }

    for (const auto &kv : this->timers) {
        JS_FreeValue(this->ctx, kv.second.js_func);
    }

    this->registered_functions.clear();

    JS_FreeValue(this->ctx, this->global_json_obj);
    JS_FreeContext(this->ctx);
}

JNIEnv* Context::getJNIEnv() {
    JNIEnv *env = nullptr;

    int status = this->vm->GetEnv((void**)&env, JNI_VERSION_1_6);
    if (status == JNI_EDETACHED) {
        if (this->vm->AttachCurrentThread(&env, NULL) != JNI_OK) {
            // TODO: throw exception here
            return nullptr;
        }
    }

    if (status == JNI_EVERSION) {
        // TODO: throw exception here
        return nullptr;
    }

    return env;
}

void Context::start_run_loop() {
    this->end_run_loop = false;

    this->run_loop_thread = std::thread([this] { run_loop(); });

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

    while (!this->run_loop_stopped) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void Context::run_loop() {
    for (;;) {
        this->run_loop_mutex.lock();
        if (this->end_run_loop) {
            break;
        }
        this->run_loop_mutex.unlock();

        if (!this->timers.empty()) {
            this->timers_mutex.lock();
            JSValue global_obj = JS_GetGlobalObject(this->ctx);

            for (const auto &kv : this->timers) {
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

JSValue Context::parseJSON(const std::string &json_string) const {
    JSValue ret_value = JS_UNDEFINED;

    JSValue global_obj = JS_GetGlobalObject(this->ctx);
    JSValue parse_func_obj = JS_GetPropertyStr(this->ctx, this->global_json_obj, "parse");

    JSValue json = JS_NewString(this->ctx, json_string.c_str());

    JSValue parsed = JS_Call(this->ctx, parse_func_obj, global_obj, 1, &json);

    // look for __cbr:: and replace with JSFunction
    JSPropertyEnum *properties;
    uint32_t count;
    JS_GetOwnPropertyNames(this->ctx, &properties, &count, parsed, JS_GPN_STRING_MASK | JS_GPN_SYMBOL_MASK | JS_GPN_ENUM_ONLY);
    for (uint32_t i = 0; i < count; i++) {
        JSAtom atom = properties[i].atom;
        const char *key = JS_AtomToCString(ctx, atom);

        JSValue val = JS_GetProperty(ctx, parsed, atom);

        if (JS_IsString(val)) {
            const char *c_str_val = JS_ToCString(ctx, val);
            std::string str_value = std::string(c_str_val);

            std::string prefix = str_value.substr(0, 7);

            if (prefix == "__cbr::") {
                try {
                    auto global_func_name = str_value.substr(7);
                    auto func_data = this->registered_functions.at(global_func_name);

                    JSValue ptr[1] = { JS_NewString(this->ctx, global_func_name.c_str()) };

                    JS_SetPropertyStr(this->ctx, parsed, key, JS_NewCFunctionData(this->ctx, call_global_function, 1, 0, 1,
                                                                                  ptr));
                } catch (std::exception &ex) {
                }
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

std::string Context::stringifyJSON(JSValue object) const {
    JSValue global_obj = JS_GetGlobalObject(this->ctx);
    JSValue stringify_func_obj = JS_GetPropertyStr(this->ctx, this->global_json_obj, "stringify");

    JSValue str_value = JS_Call(this->ctx, stringify_func_obj, global_obj, 1, &object);

    const char *c_str = JS_ToCString(this->ctx, str_value);

    std::string json(c_str);

    JS_FreeCString(this->ctx, c_str);

    JS_FreeValue(this->ctx, str_value);
    JS_FreeValue(this->ctx, stringify_func_obj);
    JS_FreeValue(this->ctx, global_obj);

    return json;
}

JSValue Context::evaluate(const std::string &code, bool ret_val) const {
    JSValue ret_value = JS_UNDEFINED;

    int flags = JS_EVAL_TYPE_GLOBAL | JS_EVAL_FLAG_BACKTRACE_BARRIER;

    size_t code_len = strlen(code.c_str());

    JSValue value = JS_Eval(this->ctx, code.c_str(), code_len, "<code>", flags);

    if (JS_IsException(value)) {
        ret_value = JS_DupValue(this->ctx, value);
        JS_FreeValue(this->ctx, value);

        return ret_value;
    }

    if (ret_val) {
        std::string json = this->stringifyJSON(value);

        JSValue json_value = JS_NewString(this->ctx, json.c_str());
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

void Context::init_api_console() const {
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

void Context::init_api_event_listeners() const {
    JSValue global_obj = JS_GetGlobalObject(this->ctx);

    JS_SetPropertyStr(this->ctx, global_obj, "addEventListener", JS_NewCFunction(this->ctx, api_add_event_listener, "addEventListener", 2));

    JS_FreeValue(this->ctx, global_obj);
}

void Context::init_api_crypto() const {
    JSValue global_obj, crypto;

    global_obj = JS_GetGlobalObject(this->ctx);

    crypto = JS_NewObject(this->ctx);

    JS_SetPropertyStr(this->ctx, crypto, "getRandomValues", JS_NewCFunction(ctx, api_crypto_get_random_values, "getRandomValues", 1));
    JS_SetPropertyStr(this->ctx, crypto, "randomUUID", JS_NewCFunction(ctx, api_crypto_random_uuid, "randomUUID", 0));

    JS_SetPropertyStr(this->ctx, global_obj, "crypto", crypto);

    JS_FreeValue(this->ctx, global_obj);
}

void Context::init_api_timeout() const {
    JSValue global_obj = JS_GetGlobalObject(this->ctx);

    JS_SetPropertyStr(this->ctx, global_obj, "setTimeout", JS_NewCFunction(this->ctx, api_set_timeout, "setTimeout", 2));
    JS_SetPropertyStr(this->ctx, global_obj, "clearTimeout", JS_NewCFunction(this->ctx, api_clear_timeout, "clearTimeout", 1));
    JS_SetPropertyStr(this->ctx, global_obj, "setInterval", JS_NewCFunction(this->ctx, api_set_interval, "setInterval", 1));
    JS_SetPropertyStr(this->ctx, global_obj, "clearInterval", JS_NewCFunction(this->ctx, api_clear_timeout, "clearInterval", 1));

    JS_FreeValue(this->ctx, global_obj);
}

void Context::init_api_text() const {
    init_text_encoder_class(this->ctx);
    init_text_decoder_class(this->ctx);
}

void Context::init_api_fetch() const {
    JSValue global_obj = JS_GetGlobalObject(this->ctx);

    JS_SetPropertyStr(this->ctx, global_obj, "fetch", JS_NewCFunction(this->ctx, api_fetch_promise, "fetch", 2));

    JS_FreeValue(this->ctx, global_obj);
}

void Context::register_function(const std::string &func_name, jobject func) {
    JavaFunctionData func_data = JavaFunctionData(func_name, func);
    this->registered_functions.insert_or_assign(func_name, func_data);
}

JSValue call_global_function(JSContext *ctx, JSValue this_val, int argc, JSValue *argv, int magic, JSValue *func_data) {
    JSValue ret_value = JS_UNDEFINED;
    JSValue jni_exception;

    JNIEnv *thread_env = nullptr;
    JSValue func_name_obj = func_data[0];

    auto func_name = JS_ToCString(ctx, func_name_obj);
    auto func_name_str = std::string(func_name);
    JS_FreeCString(ctx, func_name);

    auto *parent_ctx = (Context *)JS_GetContextOpaque(ctx);

    auto status = parent_ctx->vm->GetEnv((void**)&thread_env, JNI_VERSION_1_6);
    if (status == JNI_EDETACHED) {
        if (parent_ctx->vm->AttachCurrentThread(&thread_env, NULL) != JNI_OK) {
            return JS_EXCEPTION;
        }
    }

    auto j_func_data = parent_ctx->registered_functions.at(func_name);

    jclass j_function_class = thread_env->GetObjectClass(j_func_data.j_func);
    jni_exception = check_and_throw_jni_exception(thread_env, ctx);

    if (JS_IsException(jni_exception)) {
        return jni_exception;
    }

    jmethodID j_method = thread_env->GetMethodID(j_function_class, "run", "()V");
    jni_exception = check_and_throw_jni_exception(thread_env, ctx);

    if (JS_IsException(jni_exception)) {
        return jni_exception;
    }

    JSValue args = argv[0];
    if (!JS_IsNull(args) && !JS_IsUndefined(args)) {
        auto json_string = parent_ctx->stringifyJSON(args);
        jstring j_json_string = thread_env->NewStringUTF(json_string.c_str());

        // create a JSONObject
        jclass json_object_c = thread_env->FindClass("org/json/JSONObject");
        jmethodID json_object_cnstrctr = thread_env->GetMethodID(json_object_c, "<init>", "(Ljava/lang/String;)V");

        jobject json_object = thread_env->NewObject(json_object_c, json_object_cnstrctr, j_json_string);

        jfieldID args_field = thread_env->GetFieldID(j_function_class, "args", "Lorg/json/JSONObject;");

        thread_env->SetObjectField(j_func_data.j_func, args_field, json_object);
    }

    thread_env->CallVoidMethod(j_func_data.j_func, j_method);

    return ret_value;
}
