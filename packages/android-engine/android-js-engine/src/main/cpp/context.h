#ifndef CAPACITOR_BACKGROUND_RUNNER_CONTEXT_H
#define CAPACITOR_BACKGROUND_RUNNER_CONTEXT_H
#include <string>
#include <unordered_map>

#include <jni.h>

#include "./quickjs/quickjs.h"

#include "java.h"
#include "api/api_console.h"
#include "api/api_timeout.h"
#include "api/api_events.h"
#include "api/api_crypto.h"
#include "api/api_text.h"
#include "api/api_fetch.h"
#include "api/api_js_response.h"
#include "api/api_blob.h"

class Context {
public:
    std::string name;

    JSContext *qjs_context;
    Java *java;

    std::unordered_map<std::string, JSValue> event_listeners;
    std::unordered_map<std::string, jobject> registered_functions;
    std::unordered_map<int, Timer> timers;

    Context(const std::string& name, JSRuntime *parent_rt, JNIEnv *env);
    ~Context();

    void register_function(const std::string& func_name, jobject func);
    JSValue evaluate(const std::string& code, bool ret_val) const;
    JSValue dispatch_event(const std::string& event, JSValue details);

    void init_capacitor_api(jobject cap_api_obj);

private:
    void init_callbacks(JSValue callbacks) const;
    void log_debug(const std::string& msg) const;

    void init_api_console() const;
    void init_api_event_listeners() const;
    void init_api_timeout() const;
    void init_api_crypto() const;
    void init_api_text() const;
    void init_api_fetch() const;
    void init_api_blob() const;
};

#endif //CAPACITOR_BACKGROUND_RUNNER_CONTEXT_H
