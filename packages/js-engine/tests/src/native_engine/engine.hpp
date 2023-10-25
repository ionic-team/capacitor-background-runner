#ifndef ENGINE_H
#define ENGINE_H

#include "../../../src/context.hpp"
#include "../../../src/runner.hpp"
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
  void start();
  void stop();
};

#endif  // ENGINE_H