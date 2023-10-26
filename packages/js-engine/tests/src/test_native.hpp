#ifndef TEST_NATIVE_H
#define TEST_NATIVE_H
#include <fmt/core.h>

#include <unordered_map>

#include "../../src/native.hpp"

class TestNative : public NativeInterface {
 public:
  TestNative();
  ~TestNative();

  std::unordered_map<LoggerLevel, std::string> loggerStr;

  virtual void logger(LoggerLevel level, const std::string& tag, const std::string& messages);
  virtual void register_native_function(const std::string& func_name, void* func_obj);
  virtual JSValue invoke_native_function(const std::string& func_name, JSValue args);

 private:
  std::unordered_map<std::string, std::function<JSValue(JSValue)>> functions;
};

#endif  // TEST_NATIVE_H