#ifndef NATIVE_INTERFACE_H
#define NATIVE_INTERFACE_H

#include <fmt/core.h>

#include <nlohmann/json.hpp>
#include <unordered_map>

#include "../../../src/native.hpp"
#include "errors.hpp"

class Native : public NativeInterface {
 public:
  Native();
  ~Native();

  virtual void logger(LoggerLevel level, const std::string& tag, const std::string& messages);
  virtual void register_native_function(const std::string& func_name, std::any func_obj);
  virtual bool has_native_function(const std::string& func_name);
  virtual JSValue invoke_native_function(const std::string& func_name, JSContext* ctx, JSValue args);
  virtual std::string crypto_get_random_uuid();
  std::vector<uint8_t> crypto_get_random(size_t size);
  virtual int get_random_hash();
  virtual NativeResponse fetch(NativeRequest request);
  virtual std::string byte_array_to_str(uint8_t* arr, size_t size, const std::string& encoding);
  virtual std::vector<uint8_t> string_to_byte_array(std::string str);

 private:
  std::unordered_map<LoggerLevel, std::string> logger_str;
  std::unordered_map<std::string, std::function<nlohmann::json(nlohmann::json)>> functions;
};

#endif  // NATIVE_INTERFACE_H