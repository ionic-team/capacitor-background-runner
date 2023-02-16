#include "quickjs/quickjs.h"
#include <string>
#include <unordered_map>
#include <jni.h>
#include <thread>

#include "api_events.h"
#include "api_console.h"
#include "api_crypto.h"
#include "api_timeout.h"
#include "api_text.h"

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

    std::mutex timers_mutex;

    std::string evaluate(const std::string& code, bool ret_val) const;
    JSValue dispatch_event(const std::string& event, JSValue details);

    JSValue parseJSON(const std::string& json_string) const;
    std::string stringifyJSON(JSValue object) const;

private:
    JSValue global_json_obj;

    std::thread run_loop_thread;
    std::mutex run_loop_mutex;

    bool run_loop_stopped;
    bool end_run_loop;

    void run_loop();

    void init_api_console() const;
    void init_api_event_listeners() const;
    void init_api_crypto() const;
    void init_api_timeout() const;
    void init_api_text() const;
};

#endif //ANDROID_ENGINE_CONTEXT_H
