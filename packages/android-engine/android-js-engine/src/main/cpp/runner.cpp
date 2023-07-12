#include "runner.h"

Runner::Runner() {
    this->rt = JS_NewRuntime();
    JS_SetCanBlock(rt, 1);
    JS_SetMaxStackSize(rt, 0);
}

Runner::~Runner() {
    this->stop_run_loop();
    JS_FreeRuntime(this->rt);
    this->rt = nullptr;
}

void Runner::start_run_loop() {
    this->end_run_loop = false;

    this->run_loop_thread = std::thread([this] { run_loop(); });
    this->run_loop_thread.detach();

    this->run_loop_stopped = false;
}

void Runner::stop_run_loop() {
    if (this->end_run_loop) {
        return;
    }

    this->run_loop_mutex.lock();
    this->end_run_loop = true;
    this->run_loop_mutex.unlock();

    while (!this->run_loop_stopped) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void Runner::run_loop() {
    JSContext *job_ctx;

    for (;;) {
        this->run_loop_mutex.lock();
        if (this->end_run_loop) {
            break;
        }
        this->run_loop_mutex.unlock();

        int res = JS_ExecutePendingJob(this->rt, &job_ctx);


    }

    this->run_loop_stopped = true;
}