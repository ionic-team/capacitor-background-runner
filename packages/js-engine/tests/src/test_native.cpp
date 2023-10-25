#include "test_native.hpp"

TestNative::TestNative() {
  this->loggerStr[LoggerLevel::INFO] = "INFO";
  this->loggerStr[LoggerLevel::DEBUG] = "DEBUG";
  this->loggerStr[LoggerLevel::WARN] = "WARN";
  this->loggerStr[LoggerLevel::ERROR] = "ERR";
}

TestNative::~TestNative() {}

void TestNative::logger(LoggerLevel level, const std::string& tag, const std::string& messages) { fmt::println("{0} [{1}] {2}", this->loggerStr[level], tag, messages); }