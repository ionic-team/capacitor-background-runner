#include "native_interface.hpp"

#include <sodium.h>
#include <stduuid/uuid.h>

#include <cmath>
#include <random>

Native::Native() {
  if (sodium_init() < 0) {
    this->logger(LoggerLevel::WARN, "[NATIVE SYSTEM]", "libsodium could not be initialized");
  }
  this->logger_str[LoggerLevel::INFO] = "INFO";
  this->logger_str[LoggerLevel::DEBUG] = "DEBUG";
  this->logger_str[LoggerLevel::WARN] = "WARN";
  this->logger_str[LoggerLevel::ERROR] = "ERR";
}

Native::~Native() {}

void Native::logger(LoggerLevel level, const std::string& tag, const std::string& messages) { fmt::println("{0} [{1}] {2}", this->logger_str[level], tag, messages); }

void Native::register_native_function(const std::string& func_name, void* func_obj) {
  auto func = (std::function<nlohmann::json(nlohmann::json)>*)func_obj;
  this->functions[func_name] = *func;
}

bool Native::has_native_function(const std::string& func_name) { return this->functions.find(func_name) != this->functions.end(); }

JSValue Native::invoke_native_function(const std::string& func_name, JSContext* ctx, JSValue args) {
  JSValue js_ret = JS_UNDEFINED;
  nlohmann::json arg_json;

  auto func = functions.at(func_name);

  if (!JS_IsNull(args) && !JS_IsUndefined(args)) {
    auto arg_json_js = JS_JSONStringify(ctx, args, JS_UNDEFINED, JS_UNDEFINED);
    auto arg_c_json = JS_ToCString(ctx, arg_json_js);

    arg_json = nlohmann::json::parse(std::string(arg_c_json));

    JS_FreeCString(ctx, arg_c_json);
    JS_FreeValue(ctx, arg_json_js);
  }

  try {
    auto json_ret = func(arg_json);

    if (json_ret != nullptr && !json_ret.empty()) {
      auto json_str = json_ret.dump();
      js_ret = JS_ParseJSON(ctx, json_str.c_str(), json_str.length(), "<parse>");
    }

    return js_ret;
  } catch (std::exception& ex) {
    auto exception = JS_NewError(ctx);
    JS_SetPropertyStr(ctx, exception, "message", JS_NewString(ctx, ex.what()));

    return JS_Throw(ctx, exception);
  }
}

std::string Native::crypto_get_random_uuid() {
  // WARNING: Just for testing purposes only
  std::random_device rnd_device;
  uuids::basic_uuid_random_generator<std::random_device> generator{rnd_device};

  uuids::uuid id = generator();

  return uuids::to_string(id);
}

std::vector<uint8_t> Native::crypto_get_random(size_t size) {
  char* random_bytes[size];
  randombytes_buf(random_bytes, size);

  return std::vector<uint8_t>();
}

int Native::get_random_hash() {
  int hash = std::hash<std::string>{}(crypto_get_random_uuid());
  return abs(hash);
}