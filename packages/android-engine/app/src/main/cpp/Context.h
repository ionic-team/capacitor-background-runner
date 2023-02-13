#include "quickjs/quickjs.h"
#include <string>
#include <unordered_map>
#include <jni.h>

#ifndef ANDROID_ENGINE_CONTEXT_H
#define ANDROID_ENGINE_CONTEXT_H


class Context {
public:
    std::string name;
    JSContext *ctx;
    JNIEnv *env;

    std::unordered_multimap<std::string, JSValue> event_listeners;

    Context(const std::string& name, JSRuntime* rt, JNIEnv *env);
    ~Context();

    JSValue evaluate(const char* code);
    JSValue dispatch_event(const std::string& event, JSValue details);

    JSValue parseJSON(const char* code);
    const char * stringifyJSON(JSValue object);

private:
    JSValue global_json_obj;

    void init_api_console();
    void init_api_event_listeners();
    void init_api_crypto();
};

#endif //ANDROID_ENGINE_CONTEXT_H
