#include "runner.h"

#include "api/api_console.h"

Runner::Runner() {
    this->rt = JS_NewRuntime();
    JS_SetCanBlock(rt, 0);
    JS_SetMaxStackSize(rt, 0);

    this->log_debug("created runner");
}

Runner::~Runner() {
    this->log_debug("destroying runner");

    this->stop();

    if (!this->contexts.empty()) {
        for (const auto &kv : this->contexts) {
            this->destroy_context(kv.first);
        }
    }

    this->contexts.clear();

    this->log_debug("freeing runtime");

    JS_FreeRuntime(this->rt);
    this->rt = nullptr;

    this->log_debug("destroyed runner");
}

void Runner::start() {
    this->log_debug("starting runner run loop...");

    this->run_loop_started = true;
    this->stop_run_loop = false;

    this->run_loop();
}

void Runner::stop() {
    if (this->stop_run_loop) {
        return;
    }

    this->stop_run_loop = true;

    while(this->run_loop_started) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    this->log_debug("stopped runner run loop");
}

long Runner::create_context(std::string name, JNIEnv *env) {
    auto *context = new Context(name, this->rt, env);
    this->contexts.insert_or_assign(name, context);

    return (long)context;
}

void Runner::destroy_context(std::string name) {
    try {
        this->log_debug("try to destroy context " + name);
        auto *context = this->contexts.at(name);
        delete context;

        this->contexts.erase(name);
    } catch(std::exception& ex) {}
}

void Runner::run_loop() {
    JSContext *job_ctx;

    for (;;) {
        if (this->stop_run_loop && !JS_IsJobPending(this->rt)) {
            break;
        }

        if (JS_IsJobPending(this->rt)) {
            int const status = JS_ExecutePendingJob(this->rt, &job_ctx);
            if (status < 0) {
                auto exception_val = JS_GetException(job_ctx);
                auto err_message = JS_GetPropertyStr(job_ctx, exception_val, "message");
                const char *err_str = JS_ToCString(job_ctx, err_message);

                this->log_debug(std::string(err_str));

                JS_FreeValue(job_ctx, exception_val);
                JS_FreeCString(job_ctx, err_str);
                JS_FreeValue(job_ctx, err_message);
            }
        }

        for (const auto &kv : this->contexts) {
            kv.second->run_loop();
        }
    }

    this->run_loop_started = false;
}

void Runner::log_debug(const std::string& msg) {
    write_to_logcat(ANDROID_LOG_DEBUG, "[Runner]", msg.c_str());
}
