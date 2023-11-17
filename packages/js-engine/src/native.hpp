#ifndef CAPACITOR_BACKGROUND_NATIVE_H
#define CAPACITOR_BACKGROUND_NATIVE_H

#include <string>
#include <unordered_map>
#include <vector>

#include "quickjs/quickjs.h"

enum LoggerLevel { INFO, WARN, ERROR, DEBUG };

struct NativeResponse {
  bool ok;
  int status;
  std::string url;
  std::string error;
  std::vector<uint8_t> data;
};

struct NativeRequest {
  std::string url;
  std::string method;
  std::unordered_map<std::string, std::string> headers;
  std::vector<uint8_t> body;
};

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
  virtual std::string crypto_get_random_uuid() = 0;
  virtual std::vector<uint8_t> crypto_get_random(size_t size) = 0;
  virtual int get_random_hash() = 0;
  virtual NativeResponse fetch(NativeRequest request) = 0;
  virtual std::string byte_array_to_str(uint8_t* arr) = 0;

 protected:
  NativeInterface() = default;
};

#endif  // CAPACITOR_BACKGROUND_NATIVE_H