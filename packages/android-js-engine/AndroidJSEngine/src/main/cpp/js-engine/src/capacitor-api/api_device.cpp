#include "api_device.h"

#include "../context.hpp"

JSValue api_device_battery(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  auto *context = (Context *)JS_GetContextOpaque(ctx);

  try {
    auto json_result = context->capacitor_interface->device_api_getBatteryStatus();
    if (json_result.empty()) {
      return JS_NULL;
    }

    return JS_ParseJSON(ctx, json_result.c_str(), strlen(json_result.c_str()), "<parse battery status>");
  } catch (std::exception &ex) {
    auto js_error = create_js_error(ex.what(), ctx);
    return JS_Throw(ctx, js_error);
  }
}

JSValue api_device_network(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  auto *context = (Context *)JS_GetContextOpaque(ctx);

  try {
    auto json_result = context->capacitor_interface->device_api_getNetworkStatus();
    if (json_result.empty()) {
      return JS_NULL;
    }

    return JS_ParseJSON(ctx, json_result.c_str(), strlen(json_result.c_str()), "<parse network status>");
  } catch (std::exception &ex) {
    auto js_error = create_js_error(ex.what(), ctx);
    return JS_Throw(ctx, js_error);
  }
}