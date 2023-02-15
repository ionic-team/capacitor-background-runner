#include "quickjs/quickjs.h"
#include <string>
#include <unordered_map>
#include <jni.h>

#include "api_events.h"
#include "api_console.h"
#include "api_crypto.h"
#include "api_timeout.h"

#ifndef ANDROID_ENGINE_CONTEXT_H
#define ANDROID_ENGINE_CONTEXT_H


class Context {
public:
    std::string name;

    JSContext *ctx;
    JNIEnv *env;

    std::unordered_multimap<std::string, JSValue> event_listeners;
    std::unordered_map<int, Timer> timers;

    Context(const std::string& name, JSRuntime* rt, JNIEnv *env);
    ~Context();

    void start_run_loop();
    void stop_run_loop();

    JSValue evaluate(const char* code);
    JSValue dispatch_event(const std::string& event, JSValue details);

    JSValue parseJSON(const char* code);
    const char * stringifyJSON(JSValue object);

private:
    JSValue global_json_obj;
    bool end_run_loop;

    void init_api_console();
    void init_api_event_listeners();
    void init_api_crypto();
    void init_api_timeout();
};

#endif //ANDROID_ENGINE_CONTEXT_H
