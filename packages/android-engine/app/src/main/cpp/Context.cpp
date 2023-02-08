#include "Context.h"

#include <utility>
#include <string.h>

#include "api_events.h"
#include "api_console.h"

Context::Context(const std::string& name, JSRuntime *rt) {
    this->ctx = JS_NewContext(rt);
    this->name = name;

    this->init_api_console();
    this->init_api_event_listeners();

    JS_SetContextOpaque(this->ctx, this);
}

Context::~Context() {
    for(const auto& kv: this->event_listeners) {
        JS_FreeValue(this->ctx, kv.second);
    }

    JS_FreeContext(this->ctx);
}

JSValue Context::evaluate(const char * code) {
    int flags = JS_EVAL_TYPE_GLOBAL;
    flags |= JS_EVAL_FLAG_BACKTRACE_BARRIER;

    return JS_Eval(this->ctx, code, strlen(code), "<code>", flags);
}

JSValue Context::dispatch_event(const std::string &event) {
    JSValue global_obj = JS_GetGlobalObject(this->ctx);
    JSValue callback = this->event_listeners[event];

    JSValue value = JS_Call(this->ctx, callback, global_obj, 0, nullptr);
    JSValue retValue = JS_DupValue(this->ctx, value);

    JS_FreeValue(this->ctx, value);
    JS_FreeValue(this->ctx, global_obj);

    return retValue;
}

void Context::init_api_console()
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

void Context::init_api_event_listeners()
{
    JSValue global_obj = JS_GetGlobalObject(this->ctx);

    JS_SetPropertyStr(this->ctx, global_obj, "addEventListener", JS_NewCFunction(this->ctx, api_add_event_listener, "addEventListener", 2));

    JS_FreeValue(this->ctx, global_obj);
}



