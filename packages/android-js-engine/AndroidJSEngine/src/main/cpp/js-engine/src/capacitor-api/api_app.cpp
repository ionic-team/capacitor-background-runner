#include "api_app.h"

#include "../context.hpp"

JSValue api_app_get_info(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  auto *context = (Context *)JS_GetContextOpaque(ctx);
  if (context == nullptr) {
    auto js_error = create_js_error("context is null", ctx);
    return JS_Throw(ctx, js_error);
  }

  try {
    auto json_result = context->capacitor_interface->app_api_getInfo();
    if (json_result.empty()) {
      return JS_NULL;
    }

    return JS_ParseJSON(ctx, json_result.c_str(), strlen(json_result.c_str()), "<parse app info>");
  } catch (std::exception &ex) {
    auto js_error = create_js_error(ex.what(), ctx);
    return JS_Throw(ctx, js_error);
  }
}

JSValue api_app_get_state(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  auto *context = (Context *)JS_GetContextOpaque(ctx);
  if (context == nullptr) {
    auto js_error = create_js_error("context is null", ctx);
    return JS_Throw(ctx, js_error);
  }

  try {
    auto json_result = context->capacitor_interface->app_api_getState();
    if (json_result.empty()) {
      return JS_NULL;
    }

    return JS_ParseJSON(ctx, json_result.c_str(), strlen(json_result.c_str()), "<parse app state>");
  } catch (std::exception &ex) {
    auto js_error = create_js_error(ex.what(), ctx);
    return JS_Throw(ctx, js_error);
  }
}
