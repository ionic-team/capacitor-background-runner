#include "runner.h"

#include "api/api_console.h"

Runner::Runner() {
    this->rt = JS_NewRuntime();
    JS_SetCanBlock(rt, 0);
    JS_SetMaxStackSize(rt, 0);

    this->log_debug("created runner");
}

Runner::~Runner() {
    JS_FreeRuntime(this->rt);
    this->rt = nullptr;

    this->log_debug("destroyed runner");
}

void Runner::start() {
    this->thread = std::thread([this] {
        this->log_debug("started runner run loop");
        run_loop();
    });

    this->thread.detach();
}

void Runner::stop() {
    this->log_debug("stopped runner run loop");
}

long Runner::create_context(std::string name, JNIEnv *env) {
    auto *context = new Context(name, this->rt, env);
    this->contexts.insert_or_assign(name, context);

    return (long)context;
}

void Runner::destroy_context(std::string name) {
    try {
        auto *context = this->contexts.at(name);
        delete context;

        this->contexts.erase(name);
    } catch(std::exception& ex) {}
}

void Runner::run_loop() {
    JSContext *job_ctx;

    for (;;) {

    }
}

void Runner::log_debug(std::string msg) {
    write_to_logcat(ANDROID_LOG_DEBUG, "[Runner]", msg.c_str());
}
