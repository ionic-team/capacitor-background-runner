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
    if (this->end_run_loop && !JS_IsJobPending(this->rt)) {
      this->run_loop_mutex.unlock();
      break;
    }
    this->run_loop_mutex.unlock();

    if (this->rt == nullptr) {
      this->run_loop_mutex.lock();
      this->end_run_loop = true;
      this->run_loop_mutex.unlock();
      break;
    }

    int status = JS_ExecutePendingJob(this->rt, &job_ctx);
    if (status < 0) {
      JSValue exception_val = JS_GetException(job_ctx);
      JSValue err_message = JS_GetPropertyStr(job_ctx, exception_val, "message");
      const char *err_str = JS_ToCString(job_ctx, err_message);

      JS_FreeValue(job_ctx, exception_val);
      JS_FreeCString(job_ctx, err_str);
      JS_FreeValue(job_ctx, err_message);
    }
  }

  this->run_loop_stopped = true;
}