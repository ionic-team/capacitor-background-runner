#include "runner.hpp"

#include <string>

Runner::Runner(NativeInterface *native) {
  this->native = native;
  this->rt = JS_NewRuntime();
  JS_SetCanBlock(rt, 0);
  JS_SetMaxStackSize(rt, 0);

  this->log_debug("created runner");
}

Runner::~Runner() {
  this->log_debug("destroying runner");

  this->stop();

  if (!this->contexts.empty()) {
    auto itr = this->contexts.begin();
    while (itr != this->contexts.end()) {
      itr = this->destroy_context(itr);
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

  while (this->run_loop_started) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  this->log_debug("stopped runner run loop");
}

Context *Runner::create_context(std::string name) {
  auto *context = new Context(name, this->rt, this->native);
  this->contexts.insert_or_assign(name, context);

  return context;
}

void Runner::destroy_context(std::string name) {
  try {
    this->log_debug("try to destroy context " + name);
    auto *context = this->contexts.at(name);
    delete context;

    this->contexts.erase(name);
  } catch (std::exception &ex) {
  }
}

std::unordered_map<std::string, Context *>::iterator Runner::destroy_context(std::unordered_map<std::string, Context *>::iterator itr) {
  try {
    this->log_debug("try to destroy context " + itr->first);
    auto *context = this->contexts.at(itr->first);
    delete context;

    return this->contexts.erase(itr);
  } catch (std::exception &ex) {
    return itr;
  }
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

void Runner::log_debug(const std::string &msg) { this->native->logger(DEBUG, "Runner", msg); }
