#ifndef CAPACITOR_BACKGROUND_NATIVE_H
#define CAPACITOR_BACKGROUND_NATIVE_H

#include <string>

#include "quickjs/quickjs.h"

enum LoggerLevel { INFO, WARN, ERROR, DEBUG };

class NativeInterface {
 public:
  virtual ~NativeInterface() = default;
  NativeInterface(const NativeInterface&) = delete;
  NativeInterface(NativeInterface&&) = delete;
  NativeInterface& operator=(const NativeInterface&) = delete;

  virtual void logger(LoggerLevel level, const std::string& tag, const std::string& messages) = 0;
  virtual void register_native_function(const std::string& func_name, void* func_obj) = 0;
  virtual bool has_native_function(const std::string& func_name) = 0;
  virtual JSValue invoke_native_function(const std::string& func_name, JSContext* ctx, JSValue args) = 0;

 protected:
  NativeInterface() = default;
};

#endif  // CAPACITOR_BACKGROUND_NATIVE_H