#include "quickjs/quickjs.h"
#include <string>
#include <unordered_map>

#ifndef ANDROID_ENGINE_CONTEXT_H
#define ANDROID_ENGINE_CONTEXT_H


class Context {
public:
    std::string name;
    JSContext *ctx;
    std::unordered_map<std::string, JSValue> event_listeners;

    Context(const std::string& name, JSRuntime* rt);
    ~Context();

    JSValue evaluate(const char* code);
    JSValue dispatch_event(const std::string& event);

private:
    void init_api_console();
    void init_api_event_listeners();
};

#endif //ANDROID_ENGINE_CONTEXT_H
