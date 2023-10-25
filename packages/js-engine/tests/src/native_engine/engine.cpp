#include "engine.hpp"

#include <stdexcept>

#include "../test_native.hpp"

Engine::Engine() {
  auto native = new TestNative();
  this->runner = new Runner(native);
}

Engine::~Engine() { delete this->runner; }

void Engine::create_context(const std::string& name) { this->runner->create_context(name); }

void Engine::destroy_context(const std::string& name) { this->runner->destroy_context(name); }

Value* Engine::execute(const std::string& name, const std::string& code) {
  auto context = this->runner->contexts[name];
  if (context == nullptr) {
    throw std::invalid_argument("context not found");
  }
  auto ret = context->evaluate(code, true);

  auto json_c_string = JS_ToCString(context->qjs_context, ret);
  auto json_string = std::string(json_c_string);

  JS_FreeCString(context->qjs_context, json_c_string);
  JS_FreeValue(context->qjs_context, ret);

  return new Value(json_string);
}

void Engine::start() {
  this->thread = std::thread([this] { this->runner->start(); });
  this->thread.detach();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void Engine::stop() { this->runner->stop(); }