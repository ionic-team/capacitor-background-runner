#include "context.hpp"

#include "capacitor-api/api_device.h"
#include "capacitor-api/api_geolocation.h"
#include "capacitor-api/api_kv.h"
#include "capacitor-api/api_notifications.h"
#include "quickjs/cutils.h"

Context::Context(const std::string &name, JSRuntime *parent_rt, NativeInterface *native, CapacitorInterface *cap) {
  this->name = name;
  this->qjs_context = JS_NewContext(parent_rt);
  this->native_interface = native;
  this->capacitor_interface = cap;

  JS_SetContextOpaque(this->qjs_context, this);

  this->init_api_console();
  this->init_api_event_listeners();
  this->init_api_timeout();
  this->init_api_crypto();
  this->init_api_text();
  this->init_api_fetch();
  this->init_api_blob();

  if (this->capacitor_interface != nullptr) {
    this->init_capacitor_api();
  }
}

Context::~Context() {
  for (const auto &kv : this->event_listeners) {
    JS_FreeValue(this->qjs_context, kv.second);
  }
  this->event_listeners.clear();

  for (const auto &kv : this->timers) {
    JS_FreeValue(this->qjs_context, kv.second.js_func);
  }
  this->timers.clear();

  JS_FreeContext(this->qjs_context);

  this->native_interface->logger(LoggerLevel::INFO, this->name, "Destroying context");
}

bool Context::has_timers() { return !this->timers.empty(); }

void Context::run_timers() {
  if (this->timers.empty()) {
    return;
  }

  std::vector<int> dead_timers;

  for (const auto &timer_kv : this->timers) {
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - timer_kv.second.start);

    if (elapsed.count() >= timer_kv.second.timeout) {
      this->execute_timer(timer_kv.second.js_func);
      if (timer_kv.second.repeat) {
        this->timers[timer_kv.first].start = std::chrono::system_clock::now();
      } else {
        dead_timers.push_back(timer_kv.first);
      }
    }
  }

  for (const auto hash : dead_timers) {
    JS_FreeValue(this->qjs_context, this->timers[hash].js_func);
    this->timers.erase(hash);
  }
}

static JSValue call_registered_function(JSContext *ctx, JSValue this_val, int argc, JSValue *argv, int magic, JSValue *func_data) {
  auto *context = (Context *)JS_GetContextOpaque(ctx);

  JSValue func_name_obj = func_data[0];
  auto func_name_str = std::string(JS_ToCString(ctx, func_name_obj));
  JS_FreeValue(ctx, func_name_obj);

  try {
    return context->native_interface->invoke_native_function(func_name_str, ctx, argv[0]);
  } catch (std::exception &ex) {
    auto js_error = create_js_error(ex.what(), ctx);
    return JS_Throw(ctx, js_error);
  }
}

void Context::register_function(const std::string &func_name, std::any func) {
  this->native_interface->register_native_function(func_name, func);

  auto global_obj = JS_GetGlobalObject(this->qjs_context);
  auto func_name_value = JS_NewString(this->qjs_context, func_name.c_str());

  JSValueConst ptr[1];
  ptr[0] = func_name_value;

  JS_SetPropertyStr(this->qjs_context, global_obj, func_name.c_str(), JS_NewCFunctionData(this->qjs_context, call_registered_function, 1, 0, 1, ptr));

  JS_FreeValue(this->qjs_context, func_name_value);
  JS_FreeValue(this->qjs_context, global_obj);
}

JSValue Context::evaluate(const std::string &code, bool ret_val) const {
  JSValue ret_value = JS_UNDEFINED;

  int flags = JS_EVAL_TYPE_GLOBAL | JS_EVAL_FLAG_BACKTRACE_BARRIER;
  size_t len = strlen(code.c_str());

  JSValue value = JS_Eval(this->qjs_context, code.c_str(), len, "<code>", flags);

  if (JS_IsException(value)) {
    ret_value = JS_DupValue(this->qjs_context, value);
    JS_FreeValue(this->qjs_context, value);
    return ret_value;
  }

  if (ret_val) {
    JSValue json = JS_JSONStringify(this->qjs_context, value, JS_UNDEFINED, JS_UNDEFINED);

    ret_value = JS_DupValue(this->qjs_context, json);
    JS_FreeValue(this->qjs_context, json);
  }

  JS_FreeValue(this->qjs_context, value);
  return ret_value;
}

JSValue Context::dispatch_event(const std::string &event, JSValue details) {
  JSValue ret_value = JS_UNDEFINED;

  JSValue global_obj = JS_GetGlobalObject(this->qjs_context);
  JSValue event_handler;

  try {
    event_handler = this->event_listeners.at(event);
  } catch (std::exception &ex) {
    event_handler = JS_NULL;
  }

  if (!JS_IsNull(event_handler)) {
    JSValueConst args[3];
    JSValue dataArgs = JS_GetPropertyStr(this->qjs_context, details, "dataArgs");
    JSValue callbacks = JS_GetPropertyStr(this->qjs_context, details, "callbacks");

    int nextArgIndex = 0;

    if (!JS_IsNull(callbacks) && !JS_IsUndefined(callbacks)) {
      this->init_callbacks(callbacks);

      JSValue resolveFunc = JS_GetPropertyStr(this->qjs_context, callbacks, "resolve");
      JSValue rejectFunc = JS_GetPropertyStr(this->qjs_context, callbacks, "reject");

      args[0] = resolveFunc;
      args[1] = rejectFunc;
      nextArgIndex = 2;

      JS_FreeValue(this->qjs_context, resolveFunc);
      JS_FreeValue(this->qjs_context, rejectFunc);
    }

    if (JS_IsNull(dataArgs) || JS_IsUndefined(dataArgs)) {
      dataArgs = JS_DupValue(this->qjs_context, details);
    }

    args[nextArgIndex] = dataArgs;

    JSValue value = JS_Call(this->qjs_context, event_handler, global_obj, 3, args);

    if (JS_IsException(value)) {
      ret_value = JS_DupValue(this->qjs_context, value);
      JS_FreeValue(this->qjs_context, value);
    }

    // TODO: add warning for non-null value;

    JS_FreeValue(this->qjs_context, callbacks);
    JS_FreeValue(this->qjs_context, dataArgs);
    JS_FreeValue(this->qjs_context, value);
  }

  JS_FreeValue(this->qjs_context, global_obj);

  return ret_value;
}

void Context::init_callbacks(JSValue callbacks) const {
  // look for __cbr:: and replace with JSFunction
  JSPropertyEnum *properties;
  uint32_t count;
  JS_GetOwnPropertyNames(this->qjs_context, &properties, &count, callbacks, JS_GPN_STRING_MASK | JS_GPN_SYMBOL_MASK | JS_GPN_ENUM_ONLY);
  for (uint32_t i = 0; i < count; i++) {
    JSAtom atom = properties[i].atom;
    const char *key = JS_AtomToCString(this->qjs_context, atom);
    JSValue str_val = JS_GetProperty(this->qjs_context, callbacks, atom);

    if (JS_IsString(str_val)) {
      const char *c_str_val = JS_ToCString(this->qjs_context, str_val);

      std::string const str_value = std::string(c_str_val);
      std::string const prefix = str_value.substr(0, 7);

      if (prefix == "__cbr::") {
        auto global_func_name = str_value.substr(7);

        if (this->native_interface->has_native_function(global_func_name)) {
          auto func_name_value = JS_NewString(this->qjs_context, global_func_name.c_str());

          JSValueConst ptr[1];
          ptr[0] = func_name_value;

          JS_SetPropertyStr(this->qjs_context, callbacks, key, JS_NewCFunctionData(this->qjs_context, call_registered_function, 1, 0, 1, ptr));

          JS_FreeValue(this->qjs_context, func_name_value);
        }
      }

      JS_FreeCString(this->qjs_context, c_str_val);
    }

    JS_FreeValue(this->qjs_context, str_val);
    JS_FreeAtom(this->qjs_context, atom);
    JS_FreeCString(this->qjs_context, key);
  }
}

void Context::execute_timer(JSValue timerFunc) const {
  JSValue dupFunc = JS_DupValue(this->qjs_context, timerFunc);
  auto ret = JS_Call(this->qjs_context, dupFunc, JS_UNDEFINED, 0, nullptr);
  JS_FreeValue(this->qjs_context, dupFunc);
  JS_FreeValue(this->qjs_context, ret);
}

void Context::init_api_console() const {
  JSValue global_obj, console;

  global_obj = JS_GetGlobalObject(this->qjs_context);
  console = JS_NewObject(this->qjs_context);

  JS_SetPropertyStr(this->qjs_context, console, "log", JS_NewCFunction(qjs_context, api_console_log, "log", 1));
  JS_SetPropertyStr(this->qjs_context, console, "info", JS_NewCFunction(qjs_context, api_console_log, "info", 1));
  JS_SetPropertyStr(this->qjs_context, console, "warn", JS_NewCFunction(qjs_context, api_console_warn, "warn", 1));
  JS_SetPropertyStr(this->qjs_context, console, "error", JS_NewCFunction(qjs_context, api_console_error, "error", 1));
  JS_SetPropertyStr(this->qjs_context, console, "debug", JS_NewCFunction(qjs_context, api_console_debug, "debug", 1));

  JS_SetPropertyStr(this->qjs_context, global_obj, "console", console);

  JS_FreeValue(this->qjs_context, global_obj);
}

void Context::init_api_event_listeners() const {
  JSValue global_obj = JS_GetGlobalObject(this->qjs_context);

  JS_SetPropertyStr(this->qjs_context, global_obj, "addEventListener", JS_NewCFunction(this->qjs_context, api_add_event_listener, "addEventListener", 2));

  JS_FreeValue(this->qjs_context, global_obj);
}

void Context::init_api_timeout() const {
  JSValue global_obj = JS_GetGlobalObject(this->qjs_context);

  JS_SetPropertyStr(this->qjs_context, global_obj, "setTimeout", JS_NewCFunction(this->qjs_context, api_set_timeout, "setTimeout", 2));
  JS_SetPropertyStr(this->qjs_context, global_obj, "clearTimeout", JS_NewCFunction(this->qjs_context, api_clear_timeout, "clearTimeout", 1));
  JS_SetPropertyStr(this->qjs_context, global_obj, "setInterval", JS_NewCFunction(this->qjs_context, api_set_interval, "setInterval", 1));
  JS_SetPropertyStr(this->qjs_context, global_obj, "clearInterval", JS_NewCFunction(this->qjs_context, api_clear_timeout, "clearInterval", 1));

  JS_FreeValue(this->qjs_context, global_obj);
}

void Context::init_api_crypto() const {
  JSValue global_obj, crypto;

  global_obj = JS_GetGlobalObject(this->qjs_context);

  crypto = JS_NewObject(this->qjs_context);

  JS_SetPropertyStr(this->qjs_context, crypto, "getRandomValues", JS_NewCFunction(this->qjs_context, api_crypto_get_random_values, "getRandomValues", 1));
  JS_SetPropertyStr(this->qjs_context, crypto, "randomUUID", JS_NewCFunction(this->qjs_context, api_crypto_random_uuid, "randomUUID", 0));

  JS_SetPropertyStr(this->qjs_context, global_obj, "crypto", crypto);

  JS_FreeValue(this->qjs_context, global_obj);
}

void Context::init_api_text() const {
  init_text_encoder_class(this->qjs_context);
  init_text_decoder_class(this->qjs_context);
}

void Context::init_api_fetch() const {
  init_response_class(this->qjs_context);

  JSValue global_obj = JS_GetGlobalObject(this->qjs_context);

  JS_SetPropertyStr(this->qjs_context, global_obj, "fetch", JS_NewCFunction(this->qjs_context, api_fetch, "fetch", 2));

  JS_FreeValue(this->qjs_context, global_obj);
}

void Context::init_api_blob() const { init_blob_class(this->qjs_context); }

void Context::init_capacitor_api() {
  this->init_capacitor_device_api();
  this->init_capacitor_geolocation_api();
  this->init_capacitor_kv_api();
  this->init_capacitor_notifications_api();
}

void Context::init_capacitor_device_api() const {
  JSValue global_obj, device;

  global_obj = JS_GetGlobalObject(this->qjs_context);

  device = JS_NewObject(this->qjs_context);
  JS_SetPropertyStr(this->qjs_context, device, "getBatteryStatus", JS_NewCFunction(this->qjs_context, api_device_battery, "getBatteryStatus", 0));
  JS_SetPropertyStr(this->qjs_context, device, "getNetworkStatus", JS_NewCFunction(this->qjs_context, api_device_network, "getNetworkStatus", 0));

  JS_SetPropertyStr(this->qjs_context, global_obj, "CapacitorDevice", device);

  JS_FreeValue(this->qjs_context, global_obj);
}

void Context::init_capacitor_kv_api() const {
  JSValue global_obj, kv;

  global_obj = JS_GetGlobalObject(this->qjs_context);

  kv = JS_NewObject(this->qjs_context);
  JS_SetPropertyStr(this->qjs_context, kv, "set", JS_NewCFunction(this->qjs_context, api_kv_set, "set", 2));
  JS_SetPropertyStr(this->qjs_context, kv, "get", JS_NewCFunction(this->qjs_context, api_kv_get, "get", 1));
  JS_SetPropertyStr(this->qjs_context, kv, "remove", JS_NewCFunction(this->qjs_context, api_kv_remove, "remove", 1));

  JS_SetPropertyStr(this->qjs_context, global_obj, "CapacitorKV", kv);

  JS_FreeValue(this->qjs_context, global_obj);
}

void Context::init_capacitor_notifications_api() const {
  JSValue global_obj, notifications;

  global_obj = JS_GetGlobalObject(this->qjs_context);

  notifications = JS_NewObject(this->qjs_context);
  JS_SetPropertyStr(this->qjs_context, notifications, "schedule", JS_NewCFunction(this->qjs_context, api_notifications_schedule, "schedule", 1));

  JS_SetPropertyStr(this->qjs_context, global_obj, "CapacitorNotifications", notifications);

  JS_FreeValue(this->qjs_context, global_obj);
}

void Context::init_capacitor_geolocation_api() const {
  JSValue global_obj, geolocation;

  global_obj = JS_GetGlobalObject(this->qjs_context);
  geolocation = JS_NewObject(this->qjs_context);

  JS_SetPropertyStr(this->qjs_context, geolocation, "getCurrentPosition", JS_NewCFunction(this->qjs_context, api_geolocation_current_location, "getCurrentPosition", 0));

  JS_SetPropertyStr(this->qjs_context, global_obj, "CapacitorGeolocation", geolocation);

  JS_FreeValue(this->qjs_context, global_obj);
}