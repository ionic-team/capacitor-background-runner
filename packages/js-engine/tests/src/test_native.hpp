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
};

#endif  // TEST_NATIVE_H