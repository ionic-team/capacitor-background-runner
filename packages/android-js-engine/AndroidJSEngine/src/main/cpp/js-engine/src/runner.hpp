#ifndef CAPACITOR_BACKGROUND_RUNNER_RUNNER_H
#define CAPACITOR_BACKGROUND_RUNNER_RUNNER_H

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

  Runner(NativeInterface *native);
  ~Runner();

  void start();
  void stop();

  Context *create_context(std::string name);
  void destroy_context(std::string name);
  std::unordered_map<std::string, Context *>::iterator destroy_context(std::unordered_map<std::string, Context *>::iterator delete_itr);
  void execute_jobs();

  void log_debug(const std::string &msg);

 private:
  bool run_loop_started;
  bool stop_run_loop;
};

#endif  // CAPACITOR_BACKGROUND_RUNNER_RUNNER_H
