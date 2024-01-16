#include "api_fetch.h"

#include <thread>
#include <unordered_map>

#include "../context.hpp"
#include "../errors.hpp"

std::vector<uint8_t> js_body_to_data(JSContext *ctx, JSValue body) {
  std::vector<uint8_t> data;

  if (JS_IsNull(body) || JS_IsUndefined(body) || !JS_IsString(body)) {
    return data;
  }

  const char *c_body = JS_ToCString(ctx, body);
  auto str = std::string(c_body);
  auto str_data = str.data();

  data = std::vector<uint8_t>(&str_data[0], &str_data[str.length()]);

  JS_FreeCString(ctx, c_body);

  return data;
}

std::unordered_map<std::string, std::string> js_header_to_map(JSContext *ctx, JSValue header) {
  std::unordered_map<std::string, std::string> header_map;

  if (JS_IsNull(header) || JS_IsUndefined(header) || !JS_IsObject(header)) {
    return header_map;
  }

  JSPropertyEnum *properties;
  uint32_t count;
  JS_GetOwnPropertyNames(ctx, &properties, &count, header, JS_GPN_STRING_MASK | JS_GPN_SYMBOL_MASK | JS_GPN_ENUM_ONLY);
  for (uint32_t i = 0; i < count; i++) {
    JSAtom atom = properties[i].atom;
    const char *key = JS_AtomToCString(ctx, atom);
    JSValue val = JS_GetProperty(ctx, header, atom);
    if (JS_IsString(val)) {
      const char *c_str_val = JS_ToCString(ctx, val);
      header_map[std::string(key)] = std::string(c_str_val);
      JS_FreeCString(ctx, c_str_val);
    }

    JS_FreeValue(ctx, val);
    JS_FreeAtom(ctx, atom);
    JS_FreeCString(ctx, key);
  }

  return header_map;
}

JSValue js_fetch_job(JSContext *ctx, int argc, JSValueConst *argv) {
  JSValue resolve, reject, request, options;

  resolve = argv[0];
  reject = argv[1];
  request = argv[2];
  options = argv[3];

  auto *context = (Context *)JS_GetContextOpaque(ctx);
  if (context == nullptr) {
    auto js_error = create_js_error("parent context is null", ctx);
    reject_promise(ctx, reject, js_error);
    JS_FreeValue(ctx, js_error);
    return JS_UNDEFINED;
  }

  if (JS_IsNull(request) || !JS_IsString(request)) {
    auto js_error = create_js_error("invalid url", ctx);
    reject_promise(ctx, reject, js_error);
    JS_FreeValue(ctx, js_error);
    return JS_UNDEFINED;
  }

  const auto *c_url_str = JS_ToCString(ctx, request);

  NativeRequest native_request;
  native_request.url = std::string(c_url_str);

  JS_FreeCString(ctx, c_url_str);

  context->native_interface->logger(LoggerLevel::DEBUG, "FETCH", native_request.url);

  if (!JS_IsNull(options) && !JS_IsUndefined(options)) {
    JSValue method, body, headers;

    method = JS_GetPropertyStr(ctx, options, "method");
    body = JS_GetPropertyStr(ctx, options, "body");
    headers = JS_GetPropertyStr(ctx, options, "headers");

    native_request.method = std::string(JS_ToCString(ctx, method));
    native_request.headers = js_header_to_map(ctx, headers);
    native_request.body = js_body_to_data(ctx, body);

    JS_FreeValue(ctx, method);
    JS_FreeValue(ctx, body);
    JS_FreeValue(ctx, headers);
  } else {
    native_request.method = "GET";
  }

  NativeResponse response;

  try {
    response = context->native_interface->fetch(native_request);
  } catch (std::exception &ex) {
    auto js_error = create_js_error(ex.what(), ctx);
    reject_promise(ctx, reject, js_error);
    JS_FreeValue(ctx, js_error);
    return JS_UNDEFINED;
  }

  if (!response.ok) {
    auto js_error = create_js_error(response.error.c_str(), ctx);
    reject_promise(ctx, reject, js_error);
    JS_FreeValue(ctx, js_error);
    return JS_UNDEFINED;
  }

  auto js_response = new_js_response(ctx, response);

  auto global_obj = JS_GetGlobalObject(ctx);
  JSValueConst resolve_args[1];
  resolve_args[0] = js_response;

  JS_Call(ctx, resolve, global_obj, 1, resolve_args);

  JS_FreeValue(ctx, js_response);
  JS_FreeValue(ctx, global_obj);

  return JS_UNDEFINED;
}

JSValue api_fetch(JSContext *ctx, JSValueConst this_val, int argc, JSValue *argv) {
  JSValue promise, resolving_funcs[2];
  JSValueConst args[4];

  promise = JS_NewPromiseCapability(ctx, resolving_funcs);
  if (JS_IsException(promise)) {
    return promise;
  }

  args[0] = resolving_funcs[0];
  args[1] = resolving_funcs[1];
  args[2] = JS_DupValue(ctx, argv[0]);
  args[3] = JS_DupValue(ctx, argv[1]);

  JS_EnqueueJob(ctx, js_fetch_job, 4, args);

  JS_FreeValue(ctx, resolving_funcs[0]);
  JS_FreeValue(ctx, resolving_funcs[1]);
  JS_FreeValue(ctx, argv[0]);
  JS_FreeValue(ctx, argv[1]);

  return promise;
}
