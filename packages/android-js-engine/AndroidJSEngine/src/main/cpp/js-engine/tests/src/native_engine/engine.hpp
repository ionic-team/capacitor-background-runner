#ifndef ENGINE_H
#define ENGINE_H

#include "../../../src/context.hpp"
#include "../../../src/runner.hpp"
#include "errors.hpp"
#include "value.hpp"

class Engine {
 public:
  Runner* runner;

  std::thread thread;

  Engine();
  ~Engine();

  void create_context(const std::string& name);
  void destroy_context(const std::string& name);
  Value* execute(const std::string& name, const std::string& code);
  Value* dispatch_event(const std::string& name, const std::string& event, nlohmann::json args);
  void register_function(const std::string& context_name, const std::string& func_name, std::function<nlohmann::json(nlohmann::json)> func);
  void start();
  void stop();
};

#endif  // ENGINE_H