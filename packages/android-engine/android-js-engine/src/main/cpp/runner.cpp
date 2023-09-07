#include "runner.h"

#include "api/api_console.h"

Runner::Runner() {
    this->rt = JS_NewRuntime();
    JS_SetCanBlock(rt, 0);
    JS_SetMaxStackSize(rt, 0);

    this->log_debug("created runner");
}

Runner::~Runner() {
    this->stop();

    JS_FreeRuntime(this->rt);
    this->rt = nullptr;

    this->log_debug("destroyed runner");
}

void Runner::start() {
    this->run_loop_started = true;
    this->stop_run_loop = false;

    this->thread = std::thread([this] {
        this->log_debug("started runner run loop");
        run_loop();
    });

    this->thread.detach();
}

void Runner::stop() {
    if (this->stop_run_loop) {
        return;
    }

    this->mutex.lock();
    this->stop_run_loop = true;
    this->mutex.unlock();

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
        auto *context = this->contexts.at(name);
        delete context;

        this->contexts.erase(name);
    } catch(std::exception& ex) {}
}

void Runner::run_loop() {
    JSContext *job_ctx;

    for (;;) {
        this->mutex.lock();
        if (this->stop_run_loop) {
            break;
        }
        this->mutex.unlock();

        for (const auto &kv : this->contexts) {
            auto *context = kv.second;
            if (!context->timers.empty()) {
                auto global_obj = JS_GetGlobalObject(context->qjs_context);

                // timers
                for (const auto &timer_kv : context->timers) {
                    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - timer_kv.second.start);

                    if (duration.count() >= timer_kv.second.timeout) {
                        context->timers[timer_kv.first].start = std::chrono::system_clock::now();
                        auto value = JS_Call(context->qjs_context, context->timers[timer_kv.first].js_func, global_obj, 0, nullptr);
                        JS_FreeValue(context->qjs_context, value);

                        if (!context->timers[timer_kv.first].repeat) {
                            JS_FreeValue(context->qjs_context, context->timers[timer_kv.first].js_func);
                            context->timers.erase(timer_kv.first);
                        }
                    }
                }

                JS_FreeValue(context->qjs_context, global_obj);
            }
        }
    }

    this->run_loop_started = false;
}

void Runner::log_debug(const std::string& msg) {
    write_to_logcat(ANDROID_LOG_DEBUG, "[Runner]", msg.c_str());
}
