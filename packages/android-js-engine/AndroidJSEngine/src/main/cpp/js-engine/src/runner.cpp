#include "runner.hpp"

#include <string>

Runner::Runner(NativeInterface *native) {
  this->native = native;
  this->rt = JS_NewRuntime();
  JS_SetCanBlock(rt, 0);
  JS_SetMaxStackSize(rt, 0);

  this->stop_run_loop = false;
  this->run_loop_started = false;

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

  for (;;) {
    if (this->stop_run_loop) {
      break;
    }

    this->execute_jobs();
  }

  this->run_loop_started = false;
  this->stop_run_loop = false;
}

void Runner::stop() {
    if(!this->run_loop_started) {
        return;
    }

  this->stop_run_loop = true;
  for (;;) {
    if (this->run_loop_started == false) {
      break;
    }
  }

  this->log_debug("loop stopped");
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
    // TODO:
    this->log_debug("could not destroy context " + name);
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

void Runner::execute_jobs() {
  JSContext *job_ctx;

  while (JS_IsJobPending(this->rt)) {
    int const status = JS_ExecutePendingJob(this->rt, &job_ctx);
    if (status <= 0) {
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
  }

  for (const auto &kv : this->contexts) {
    kv.second->run_loop();
  }
}

void Runner::log_debug(const std::string &msg) { this->native->logger(DEBUG, "Runner", msg); }
