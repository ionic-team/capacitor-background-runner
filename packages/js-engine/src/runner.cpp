#include "runner.hpp"

#include <string>

static void loop_prepare_callback(uv_prepare_t *handle) {}
static void loop_check_callback(uv_check_t *handle) {
  Runner *runner = (Runner *)handle->data;
  if (runner == nullptr) {
    return;
  }

  runner->execute_jobs();
}
static void loop_idle_callback(uv_idle_t *handle) {}
static void loop_async_callback(uv_async_t *handle) {
  Runner *runner = (Runner *)handle->data;
  if (runner == nullptr) {
    return;
  }

  uv_stop(runner->uv_loop);
}

Runner::Runner(NativeInterface *native) {
  this->native = native;
  this->rt = JS_NewRuntime();
  JS_SetCanBlock(rt, 0);
  JS_SetMaxStackSize(rt, 0);

  this->uv_loop = uv_default_loop();
  uv_loop_init(this->uv_loop);

  this->prepare = new uv_prepare_t;
  uv_prepare_init(this->uv_loop, this->prepare);
  this->prepare->data = this;

  this->check = new uv_check_t;
  uv_check_init(this->uv_loop, this->check);
  this->prepare->data = this;

  this->idle = new uv_idle_t;
  uv_idle_init(this->uv_loop, this->idle);
  this->idle->data = this;

  this->async = new uv_async_t;
  uv_async_init(this->uv_loop, this->async, loop_async_callback);
  this->async->data = this;

  this->log_debug("created runner");
}

Runner::~Runner() {
  this->log_debug("destroying runner");

  this->stop();

  uv_loop_close(this->uv_loop);

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

  int ret_val = 0;
  // ret_val = uv_prepare_start(this->prepare, loop_prepare_callback);
  // uv_unref((uv_handle_t *)this->prepare);
  // ret_val = uv_check_start(this->check, loop_check_callback);
  // uv_unref((uv_handle_t *)this->check);

  uv_idle_start(this->idle, loop_idle_callback);

  uv_run(this->uv_loop, UV_RUN_DEFAULT);

  this->log_debug("loop stopped");
}

void Runner::stop() { uv_async_send(this->async); }

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

void Runner::execute_jobs() {
  JSContext *job_ctx;

  for (;;) {
    int const status = JS_ExecutePendingJob(this->rt, &job_ctx);
    if (status <= 0) {
      auto exception_val = JS_GetException(job_ctx);
      auto err_message = JS_GetPropertyStr(job_ctx, exception_val, "message");
      const char *err_str = JS_ToCString(job_ctx, err_message);

      this->log_debug(std::string(err_str));

      JS_FreeValue(job_ctx, exception_val);
      JS_FreeCString(job_ctx, err_str);
      JS_FreeValue(job_ctx, err_message);
    }
  }

  // for (;;) {
  //   if (this->stop_run_loop && !JS_IsJobPending(this->rt)) {
  //     break;
  //   }

  //   if (JS_IsJobPending(this->rt)) {
  //     std::stringstream ss;
  //     auto thread_id = std::this_thread::get_id();
  //     ss << thread_id;

  //     this->log_debug("THREAD: " + ss.str());
  //     int const status = JS_ExecutePendingJob(this->rt, &job_ctx);
  //     if (status <= 0) {
  //       auto exception_val = JS_GetException(job_ctx);
  //       auto err_message = JS_GetPropertyStr(job_ctx, exception_val, "message");
  //       const char *err_str = JS_ToCString(job_ctx, err_message);

  //       this->log_debug(std::string(err_str));

  //       JS_FreeValue(job_ctx, exception_val);
  //       JS_FreeCString(job_ctx, err_str);
  //       JS_FreeValue(job_ctx, err_message);
  //     }
  //   }

  //   for (const auto &kv : this->contexts) {
  //     kv.second->run_loop();
  //   }
  // }

  // this->run_loop_started = false;
}

void Runner::log_debug(const std::string &msg) { this->native->logger(DEBUG, "Runner", msg); }
