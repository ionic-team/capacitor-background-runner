#include "context.h"
#include "quickjs/cutils.h"
#include "errors.h"

Context::Context(const std::string& name, JSRuntime *parent_rt, JNIEnv *env) {
    this->name = name;
    this->qjs_context = JS_NewContext(parent_rt);
    this->java = new Java(env);
    this->cap_api = nullptr;

    JS_SetContextOpaque(this->qjs_context, this);

    this->init_api_console();
    this->init_api_event_listeners();
    this->init_api_timeout();
    this->init_api_crypto();
    this->init_api_text();
    this->init_api_fetch();
    this->init_api_blob();

    this->log_debug("created context");
}

Context::~Context() {
    for(const auto &kv : this->event_listeners) {
        JS_FreeValue(this->qjs_context, kv.second);
    }
    this->event_listeners.clear();
    this->registered_functions.clear();

    for (const auto  &kv : this->timers) {
        JS_FreeValue(this->qjs_context, kv.second.js_func);
    }
    this->timers.clear();

    JS_FreeContext(this->qjs_context);

    this->log_debug("destroyed context");
}

void Context::run_loop() {
    if (!this->timers.empty()) {
        for (const auto &timer_kv : this->timers) {
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - timer_kv.second.start);
            if (duration.count() >= timer_kv.second.timeout) {
                this->timers[timer_kv.first].start = std::chrono::system_clock::now();
                this->execute_timer(timer_kv.second.js_func);
                if (!this->timers[timer_kv.first].repeat) {
                    JS_FreeValue(this->qjs_context, this->timers[timer_kv.first].js_func);
                    this->timers.erase(timer_kv.first);
                }
            }
        }
    }
}

static JSValue call_registered_function(JSContext *ctx, JSValue this_val, int argc, JSValue *argv, int magic, JSValue *func_data) {
    JSValue ret_value = JS_UNDEFINED;

    auto *context = (Context *)JS_GetContextOpaque(ctx);

    JSValue func_name_obj = func_data[0];
    auto func_name_str = std::string(JS_ToCString(ctx, func_name_obj));
    JS_FreeValue(ctx, func_name_obj);

    auto j_func = context->registered_functions.at(func_name_str);

    auto *env = context->java->getEnv();
    if (env == nullptr) {
        throw_js_exception(ctx, "JVM Environment is null");
        return JS_UNDEFINED;
    }

    auto j_function_class = env->GetObjectClass(j_func);
    auto exception = throw_jvm_exception_in_js(env, ctx);
    if (JS_IsException(exception)) {
        return exception;
    }

    auto j_method = env->GetMethodID(j_function_class, "run", "()V");
    exception = throw_jvm_exception_in_js(env, ctx);
    if (JS_IsException(exception)) {
        return exception;
    }

    JSValue args = argv[0];
    if (!JS_IsNull(args) && !JS_IsUndefined(args)) {
        std::string json_string;

        if (JS_IsError(context->qjs_context, args)) {
            auto error_object = JS_NewObject(ctx);

            auto error_name = JS_GetPropertyStr(ctx, args, "name");
            auto error_message = JS_GetPropertyStr(ctx, args, "message");

            JS_SetPropertyStr(ctx, error_object, "name", error_name);
            JS_SetPropertyStr(ctx, error_object, "message", error_message);

            auto json_string_obj = JS_JSONStringify(ctx, error_object, JS_UNDEFINED, JS_UNDEFINED);
            auto json_c_string = JS_ToCString(ctx, json_string_obj);

            json_string = std::string(json_c_string);

            JS_FreeCString(ctx, json_c_string);
            JS_FreeValue(ctx, json_string_obj);
            JS_FreeValue(ctx, error_object);
        } else {
            auto json_string_obj = JS_JSONStringify(ctx, args, JS_UNDEFINED, JS_UNDEFINED);
            auto json_c_string = JS_ToCString(ctx, json_string_obj);

            json_string = std::string(json_c_string);

            JS_FreeCString(ctx, json_c_string);
            JS_FreeValue(ctx, json_string_obj);
        }

        jstring j_json_string = env->NewStringUTF(json_string.c_str());

        // create a JSONObject
        jclass json_object_class = env->FindClass("org/json/JSONObject");
        jmethodID json_object_constructor = env->GetMethodID(json_object_class, "<init>", "(Ljava/lang/String;)V");
        exception = throw_jvm_exception_in_js(env, ctx);
        if (JS_IsException(exception)) {
            return exception;
        }

        jobject json_object = env->NewObject(json_object_class, json_object_constructor, j_json_string);
        exception = throw_jvm_exception_in_js(env, ctx);
        if (JS_IsException(exception)) {
            return exception;
        }

        jfieldID args_field = env->GetFieldID(j_function_class, "args", "Lorg/json/JSONObject;");
        env->SetObjectField(j_func, args_field, json_object);
    }

    env->CallVoidMethod(j_func, j_method);
    exception = throw_jvm_exception_in_js(env, ctx);
    if (JS_IsException(exception)) {
        return exception;
    }

    return ret_value;
}

void Context::register_function(const std::string& func_name, jobject func) {
    this->registered_functions.insert_or_assign(func_name, func);

    auto global_obj = JS_GetGlobalObject(this->qjs_context);
    auto func_name_value = JS_NewString(this->qjs_context, func_name.c_str());

    JSValueConst ptr[1];
    ptr[0] = func_name_value;

    JS_SetPropertyStr(this->qjs_context, global_obj, func_name.c_str(), JS_NewCFunctionData(this->qjs_context, call_registered_function, 1, 0, 1, ptr));

    JS_FreeValue(this->qjs_context, func_name_value);
    JS_FreeValue(this->qjs_context, global_obj);
}

JSValue Context::evaluate(const std::string& code, bool ret_val) const {
    JSValue ret_value = JS_UNDEFINED;

    int flags = JS_EVAL_TYPE_GLOBAL | JS_EVAL_FLAG_BACKTRACE_BARRIER;
    size_t len = strlen(code.c_str());

    write_to_logcat(ANDROID_LOG_DEBUG, "[RUNNER DEV TRACER]", "evaluate context code");
    JSValue value = JS_Eval(this->qjs_context, code.c_str(), len, "<code>", flags);

    if (JS_IsException(value)) {
        ret_value = JS_DupValue(this->qjs_context, value);
        JS_FreeValue(this->qjs_context, value);
        return ret_value;
    }

    if (ret_val) {
        JSValue json = JS_JSONStringify(this->qjs_context, value, JS_UNDEFINED, JS_UNDEFINED);

        ret_value = JS_DupValue(this->qjs_context, json);
        JS_FreeValue(this->qjs_context, json);
    }

    JS_FreeValue(this->qjs_context, value);
    return ret_value;
}

JSValue Context::dispatch_event(const std::string &event, JSValue details) {
    JSValue ret_value = JS_UNDEFINED;

    JSValue global_obj = JS_GetGlobalObject(this->qjs_context);
    JSValue event_handler;

    try {
        event_handler = this->event_listeners.at(event);
    } catch(std::exception& ex) {
        event_handler = JS_NULL;
    }

    if (!JS_IsNull(event_handler)) {
        JSValueConst args[3];
        JSValue dataArgs = JS_GetPropertyStr(this->qjs_context, details, "dataArgs");
        JSValue callbacks = JS_GetPropertyStr(this->qjs_context, details, "callbacks");

        int nextArgIndex = 0;

        if (!JS_IsNull(callbacks) && !JS_IsUndefined(callbacks)) {
            this->init_callbacks(callbacks);

            JSValue resolveFunc = JS_GetPropertyStr(this->qjs_context, callbacks, "resolve");
            JSValue rejectFunc = JS_GetPropertyStr(this->qjs_context, callbacks, "reject");

            args[0] = resolveFunc;
            args[1] = rejectFunc;
            nextArgIndex = 2;

            JS_FreeValue(this->qjs_context, resolveFunc);
            JS_FreeValue(this->qjs_context, rejectFunc);
        }

        if (JS_IsNull(dataArgs) || JS_IsUndefined(dataArgs)) {
            dataArgs = JS_DupValue(this->qjs_context, details);
        }

        args[nextArgIndex] = dataArgs;

        JSValue value = JS_Call(this->qjs_context, event_handler, global_obj, 3, args);

        if (JS_IsException(value)) {
            ret_value = JS_DupValue(this->qjs_context, value);
            JS_FreeValue(this->qjs_context, value);
        }

        // TODO: add warning for non-null value;

        JS_FreeValue(this->qjs_context, callbacks);
        JS_FreeValue(this->qjs_context, dataArgs);
        JS_FreeValue(this->qjs_context, value);
    }

    JS_FreeValue(this->qjs_context, global_obj);

    return ret_value;
}

void Context::init_capacitor_api(jobject cap_api_obj) {
    this->cap_api = cap_api_obj;

    this->init_capacitor_kv_api();
}

void Context::init_callbacks(JSValue callbacks) const {
    // look for __cbr:: and replace with JSFunction
    JSPropertyEnum *properties;
    uint32_t count;
    JS_GetOwnPropertyNames(this->qjs_context, &properties, &count, callbacks, JS_GPN_STRING_MASK | JS_GPN_SYMBOL_MASK | JS_GPN_ENUM_ONLY);
    for (uint32_t i = 0; i < count; i++) {
        JSAtom atom = properties[i].atom;
        const char *key = JS_AtomToCString(this->qjs_context, atom);
        JSValue str_val = JS_GetProperty(this->qjs_context, callbacks, atom);

        if (JS_IsString(str_val)) {
            const char *c_str_val = JS_ToCString(this->qjs_context, str_val);

            std::string const str_value = std::string(c_str_val);
            std::string const prefix = str_value.substr(0, 7);

            if (prefix == "__cbr::") {
                try {
                    auto global_func_name = str_value.substr(7);

                    if (this->registered_functions.find(global_func_name) != this->registered_functions.end()) {
                        auto func_name_value = JS_NewString(this->qjs_context, global_func_name.c_str());

                        JSValueConst ptr[1];
                        ptr[0] = func_name_value;

                        JS_SetPropertyStr(this->qjs_context, callbacks, key, JS_NewCFunctionData(this->qjs_context, call_registered_function, 1, 0, 1, ptr));

                        JS_FreeValue(this->qjs_context, func_name_value);
                    }
                } catch(std::exception& ex) {}
            }

            JS_FreeCString(this->qjs_context, c_str_val);
        }

        JS_FreeValue(this->qjs_context, str_val);
        JS_FreeAtom(this->qjs_context, atom);
        JS_FreeCString(this->qjs_context, key);
    }
}

void Context::log_debug(const std::string& msg) const {
    auto tag = "[Runner Context " + this->name + "]";
    write_to_logcat(ANDROID_LOG_DEBUG, tag.c_str(), msg.c_str());
}

void Context::execute_timer(JSValue timerFunc) const {
    write_to_logcat(ANDROID_LOG_DEBUG, "[RUNNER DEV TRACER]", "fire timer");
    JSValue dupFunc = JS_DupValue(this->qjs_context, timerFunc);
    auto ret = JS_Call(this->qjs_context,dupFunc, JS_UNDEFINED, 0, nullptr);
    JS_FreeValue(this->qjs_context, dupFunc);
    JS_FreeValue(this->qjs_context, ret);
}

void Context::init_api_console() const {
    JSValue global_obj, console;

    global_obj = JS_GetGlobalObject(this->qjs_context);
    console = JS_NewObject(this->qjs_context);

    JS_SetPropertyStr(this->qjs_context, console, "log", JS_NewCFunction(qjs_context, api_console_log, "log", 1));
    JS_SetPropertyStr(this->qjs_context, console, "info", JS_NewCFunction(qjs_context, api_console_log, "info", 1));
    JS_SetPropertyStr(this->qjs_context, console, "warn", JS_NewCFunction(qjs_context, api_console_warn, "warn", 1));
    JS_SetPropertyStr(this->qjs_context, console, "error", JS_NewCFunction(qjs_context, api_console_error, "error", 1));
    JS_SetPropertyStr(this->qjs_context, console, "debug", JS_NewCFunction(qjs_context, api_console_debug, "debug", 1));

    JS_SetPropertyStr(this->qjs_context, global_obj, "console", console);

    JS_FreeValue(this->qjs_context, global_obj);
}

void Context::init_api_event_listeners() const {
    JSValue global_obj = JS_GetGlobalObject(this->qjs_context);

    JS_SetPropertyStr(this->qjs_context, global_obj, "addEventListener", JS_NewCFunction(this->qjs_context, api_add_event_listener, "addEventListener", 2));

    JS_FreeValue(this->qjs_context, global_obj);
}

void Context::init_api_timeout() const {
    JSValue global_obj = JS_GetGlobalObject(this->qjs_context);

    JS_SetPropertyStr(this->qjs_context, global_obj, "setTimeout", JS_NewCFunction(this->qjs_context, api_set_timeout, "setTimeout", 2));
    JS_SetPropertyStr(this->qjs_context, global_obj, "clearTimeout", JS_NewCFunction(this->qjs_context, api_clear_timeout, "clearTimeout", 1));
    JS_SetPropertyStr(this->qjs_context, global_obj, "setInterval", JS_NewCFunction(this->qjs_context, api_set_interval, "setInterval", 1));
    JS_SetPropertyStr(this->qjs_context, global_obj, "clearInterval", JS_NewCFunction(this->qjs_context, api_clear_timeout, "clearInterval", 1));

    JS_FreeValue(this->qjs_context, global_obj);
}

void Context::init_api_crypto() const {
    JSValue global_obj, crypto;

    global_obj = JS_GetGlobalObject(this->qjs_context);

    crypto = JS_NewObject(this->qjs_context);

    JS_SetPropertyStr(this->qjs_context, crypto, "getRandomValues", JS_NewCFunction(this->qjs_context, api_crypto_get_random_values, "getRandomValues", 1));
    JS_SetPropertyStr(this->qjs_context, crypto, "randomUUID", JS_NewCFunction(this->qjs_context, api_crypto_random_uuid, "randomUUID", 0));

    JS_SetPropertyStr(this->qjs_context, global_obj, "crypto", crypto);

    JS_FreeValue(this->qjs_context, global_obj);
}

void Context::init_api_text() const {
    init_text_encoder_class(this->qjs_context);
    init_text_decoder_class(this->qjs_context);
}

void Context::init_api_fetch() const {
    init_response_class(this->qjs_context);

    JSValue global_obj = JS_GetGlobalObject(this->qjs_context);

    JS_SetPropertyStr(this->qjs_context, global_obj, "fetch", JS_NewCFunction(this->qjs_context, api_fetch, "fetch", 2));

    JS_FreeValue(this->qjs_context, global_obj);
}

void Context::init_api_blob() const {
    init_blob_class(this->qjs_context);
}

void Context::init_capacitor_kv_api() const {
    JSValue global_obj, kv;

    global_obj = JS_GetGlobalObject(this->qjs_context);

    kv = JS_NewObject(this->qjs_context);
    JS_SetPropertyStr(this->qjs_context, kv, "set", JS_NewCFunction(this->qjs_context, api_kv_set, "set", 2));
    JS_SetPropertyStr(this->qjs_context, kv, "get", JS_NewCFunction(this->qjs_context, api_kv_get, "get", 1));
    JS_SetPropertyStr(this->qjs_context, kv, "remove", JS_NewCFunction(this->qjs_context, api_kv_remove, "remove", 1));

    JS_SetPropertyStr(this->qjs_context, global_obj, "CapacitorKV", kv);

    JS_FreeValue(this->qjs_context, global_obj);
}