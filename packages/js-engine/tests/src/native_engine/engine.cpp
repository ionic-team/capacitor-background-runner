#include "engine.hpp"

#include <stdexcept>

#include "native_interface.hpp"

Engine::Engine() {
  auto native = new Native();
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

  if (JS_IsException(ret)) {
    JSValue const exception = JS_GetException(context->qjs_context);

    JSValue const err_message = JS_GetPropertyStr(context->qjs_context, exception, "message");
    const char* err_message_c_str = JS_ToCString(context->qjs_context, err_message);

    throw JavaScriptException(err_message_c_str);
  }

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

void Engine::register_function(const std::string& context_name, const std::string& func_name, std::function<nlohmann::json(nlohmann::json)> func) {
  auto context = this->runner->contexts[context_name];
  if (context == nullptr) {
    throw std::invalid_argument("context not found");
  }

  context->register_function(func_name, &func);
}

Value* Engine::dispatch_event(const std::string& name, const std::string& event, nlohmann::json args) {
  auto context = this->runner->contexts[name];
  if (context == nullptr) {
    throw std::invalid_argument("context not found");
  }

  // JSON to JSValue
  JSValue js_object;

  if (args != nullptr) {
    auto json_string = args.dump();
    js_object = JS_ParseJSON(context->qjs_context, json_string.c_str(), json_string.length(), "<parse>");
  } else {
    js_object = JS_NewObject(context->qjs_context);
  }

  context->dispatch_event(event, js_object);

  JS_FreeValue(context->qjs_context, js_object);

  return nullptr;
}