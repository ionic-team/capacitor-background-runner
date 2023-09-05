#include "context.h"
#include "quickjs/cutils.h"

static JSValue call_registered_function(JSContext *ctx, JSValue this_val, int argc, JSValue *argv, int magic, JSValue *func_data) {
    JSValue ret_value = JS_UNDEFINED;

    auto *context = (Context *)JS_GetContextOpaque(ctx);

    JSValue func_name_obj = func_data[0];
    auto func_name_str = std::string(JS_ToCString(ctx, func_name_obj));
    JS_FreeValue(ctx, func_name_obj);

    auto j_func = context->registered_functions.at(func_name_str);

    auto *env = context->java->getEnv();
    if (env == nullptr) {
        // TODO: throw error
    }

    auto j_function_class = env->GetObjectClass(j_func);

    return ret_value;
}

Context::Context(const std::string& name, JSRuntime *parent_rt, JNIEnv *env) {
    this->name = name;
    this->qjs_context = JS_NewContext(parent_rt);
    this->java = new Java(env);

    JS_SetContextOpaque(this->qjs_context, this);

    this->init_api_console();
    this->init_api_event_listeners();

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

void Context::register_function(const std::string& func_name, jobject func) {
    this->registered_functions.insert_or_assign(func_name, func);

    auto global_obj = JS_GetGlobalObject(this->qjs_context);

    JSValue ptr[1] = {JS_NewString(this->qjs_context, func_name.c_str())};
    JS_SetPropertyStr(this->qjs_context, global_obj, func_name.c_str(), JS_NewCFunctionData(this->qjs_context, call_registered_function, 1, 0, 1, ptr));

    JS_FreeValue(this->qjs_context, global_obj);
}

JSValue Context::evaluate(const std::string& code, bool ret_val) const {
    JSValue ret_value = JS_UNDEFINED;

    int flags = JS_EVAL_TYPE_GLOBAL | JS_EVAL_FLAG_BACKTRACE_BARRIER;
    size_t len = strlen(code.c_str());

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
            std::string str_value = std::string(c_str_val);
            std::string prefix = str_value.substr(0, 7);

            if (prefix == "__cbr::") {
                try {
                    auto global_func_name = str_value.substr(7);
                    auto func_data = this->registered_functions.at(global_func_name);
                } catch(std::exception& ex) {}
            }

            JS_FreeCString(this->qjs_context, c_str_val);
        }

        JS_FreeValue(this->qjs_context, str_val);
        JS_FreeAtom(this->qjs_context, atom);
        JS_FreeCString(this->qjs_context, key);
    }
}

void Context::log_debug(std::string msg) {
    auto tag = "[Runner Context " + this->name + "]";
    write_to_logcat(ANDROID_LOG_DEBUG, tag.c_str(), msg.c_str());
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