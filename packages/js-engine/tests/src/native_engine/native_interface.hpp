#ifndef NATIVE_INTERFACE_H
#define NATIVE_INTERFACE_H

#include <fmt/core.h>

#include <nlohmann/json.hpp>
#include <unordered_map>

#include "../../../src/native.hpp"

class Native : public NativeInterface {
 public:
  Native();
  ~Native();

  virtual void logger(LoggerLevel level, const std::string& tag, const std::string& messages);
  virtual void register_native_function(const std::string& func_name, void* func_obj);
  virtual bool has_native_function(const std::string& func_name);
  virtual JSValue invoke_native_function(const std::string& func_name, JSContext* ctx, JSValue args);

 private:
  std::unordered_map<LoggerLevel, std::string> logger_str;
  std::unordered_map<std::string, std::function<nlohmann::json(nlohmann::json)>> functions;
};

#endif  // NATIVE_INTERFACE_H