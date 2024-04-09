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

  bool has_pending_jobs();
  void execute_pending_jobs();

  Context *create_context(std::string name, CapacitorInterface *cap_api);
  void destroy_context(std::string name);
  std::unordered_map<std::string, Context *>::iterator destroy_context(std::unordered_map<std::string, Context *>::iterator delete_itr);

  void log_debug(const std::string &msg);

 private:
  bool has_active_timers();
};

#endif  // CAPACITOR_BACKGROUND_RUNNER_RUNNER_H
