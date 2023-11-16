#ifndef CAPACITOR_BACKGROUND_RUNNER_RUNNER_H
#define CAPACITOR_BACKGROUND_RUNNER_RUNNER_H

#include <uv.h>

#include <queue>
#include <string>
#include <thread>
#include <unordered_map>

#include "./quickjs/quickjs.h"
#include "context.hpp"
#include "native.hpp"

class Runner {
 public:
  JSRuntime *rt;
  std::unordered_map<std::string, Context *> contexts;
  NativeInterface *native;
  uv_loop_t *uv_loop;

  Runner(NativeInterface *native);
  ~Runner();

  void start();
  void stop();

  Context *create_context(std::string name);
  void destroy_context(std::string name);
  std::unordered_map<std::string, Context *>::iterator destroy_context(std::unordered_map<std::string, Context *>::iterator delete_itr);
  void execute_jobs();

 private:
  bool run_loop_started;
  bool stop_run_loop;

  // TODO: Rename these
  uv_check_t *check;
  uv_prepare_t *prepare;
  uv_idle_t *idle;
  uv_async_t *async;

  void log_debug(const std::string &msg);
};

// TODO: Rename these
static void loop_prepare_callback(uv_prepare_t *handle);
static void loop_check_callback(uv_check_t *handle);
static void loop_idle_callback(uv_idle_t *handle);
static void loop_async_callback(uv_async_t *handle);

#endif  // CAPACITOR_BACKGROUND_RUNNER_RUNNER_H
