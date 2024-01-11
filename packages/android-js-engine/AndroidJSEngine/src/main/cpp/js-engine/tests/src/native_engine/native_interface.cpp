#define BOOST_NETWORK_ENABLE_HTTPS

#include "native_interface.hpp"

#include <cpr/cpr.h>
#include <sodium.h>
#include <stduuid/uuid.h>

#include <cmath>
#include <random>
#include <vector>

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

void Native::register_native_function(const std::string& func_name, std::any func_obj) {
  auto unwrapped_func_obj = std::any_cast<std::function<nlohmann::json(nlohmann::json)>>(func_obj);
  this->functions[func_name] = unwrapped_func_obj;
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
  uint8_t* random_buf = new uint8_t[size];
  randombytes_buf(random_buf, size);

  return std::vector<uint8_t>(random_buf, random_buf + int(size));
}

int Native::get_random_hash() {
  int hash = std::hash<std::string>{}(crypto_get_random_uuid());
  return abs(hash);
}

NativeResponse Native::fetch(NativeRequest native_request) {
  NativeResponse native_response;

  try {
    cpr::Response res;

    cpr::Header headers = cpr::Header{{}};
    for (const auto& kv : native_request.headers) {
      headers[kv.first] = kv.second;
    }

    if (native_request.method == "GET") {
      res = cpr::Get(cpr::Url(native_request.url.c_str()), headers);
    } else if (native_request.method == "POST") {
      res = cpr::Post(cpr::Url(native_request.url.c_str()), cpr::Body(std::string(native_request.body.begin(), native_request.body.end())), headers);
    } else {
      throw new FetchException("invalid HTTP Method");
    }

    auto body_data = res.text.data();

    if (res.status_code == 0) {
      native_response.ok = false;
      native_response.status = res.status_code;
      native_response.url = native_request.url;
      native_response.error = "invalid network request";

    } else {
      native_response.ok = true;
      native_response.status = res.status_code;
      native_response.url = native_request.url;
      native_response.data = std::vector<uint8_t>(&body_data[0], &body_data[res.text.length()]);
    }
  } catch (std::exception& ex) {
    native_response.ok = false;
    native_response.status = 500;
    native_response.error = std::string(ex.what());
  }

  // httplib::Client client(fmt::format("{}//{}", url->get_protocol(), url->get_host()));
  // client.set_ca_cert_path("/opt/homebrew/etc/openssl@3/cert.pem");
  // auto path = url->get_pathname();

  // if (auto res = client.Get(std::string(path))) {
  //   auto body_data = res->body.data();

  //   response.status = res->status;
  //   response.data = std::vector<uint8_t>(&body_data[0], &body_data[res->body.length()]);
  //   response.url = request.url;
  //   response.ok = true;
  // } else {
  //   response.ok = false;
  //   response.status = res->status;
  //   auto error_message = httplib::to_string(res.error());
  //   response.error = error_message;
  // }

  return native_response;
}

std::string Native::byte_array_to_str(uint8_t* arr, size_t size, const std::string& encoding) { return std::string((char*)arr); }

std::vector<uint8_t> Native::string_to_byte_array(std::string str) { return std::vector<uint8_t>(str.begin(), str.end()); }