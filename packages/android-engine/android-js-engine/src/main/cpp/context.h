#ifndef CAPACITOR_BACKGROUND_RUNNER_CONTEXT_H
#define CAPACITOR_BACKGROUND_RUNNER_CONTEXT_H
#include <jni.h>
#include "quickjs/quickjs.h"
#include <string>
#include <unordered_map>

#include <thread>
#include <vector>
#include <queue>

#include "api_events.h"
#include "api_console.h"
#include "api_crypto.h"
#include "api_timeout.h"
#include "api_text.h"
#include "api_fetch.h"

JSValue call_global_function(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);

class Context {
public:
    std::string name;

    JSContext *ctx;
    JNIEnv *env;
    JavaVM *vm;

    jobject api;

    std::unordered_multimap<std::string, JSValue> event_listeners;
    std::unordered_map<int, Timer> timers;
    std::unordered_map<std::string, int> function_index;
    std::vector<jobject> functions;
    std::queue<FetchPromise> fetches;

    Context(const std::string& name, JSRuntime* rt, JNIEnv *env, jobject instance);
    ~Context();

    void start_run_loop();
    void stop_run_loop();

    std::mutex timers_mutex;
    std::mutex fetch_mutex;

    JSValue evaluate(const std::string& code, bool ret_val) const;
    JSValue dispatch_event(const std::string& event, JSValue details);
    void register_function(const std::string& func_name, jobject func);

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
    void init_api_fetch() const;
//    void init_api_kv() const;
};

#endif //CAPACITOR_BACKGROUND_RUNNER_CONTEXT_H
