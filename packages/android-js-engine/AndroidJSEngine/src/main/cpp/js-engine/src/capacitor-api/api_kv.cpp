#include "api_kv.h"

#include "../context.hpp"

JSValue api_kv_set(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  auto *context = (Context *)JS_GetContextOpaque(ctx);
  if (context == nullptr) {
    auto js_error = create_js_error("context is null", ctx);
    return JS_Throw(ctx, js_error);
  }

  const auto *key_c_str = JS_ToCString(ctx, argv[0]);
  const auto *value_c_str = JS_ToCString(ctx, argv[1]);

  try {
    context->capacitor_interface->kv_api_set(key_c_str, value_c_str);

    JS_FreeCString(ctx, key_c_str);
    JS_FreeCString(ctx, value_c_str);

    return JS_UNDEFINED;
  } catch (std::exception &ex) {
    JS_FreeCString(ctx, key_c_str);
    JS_FreeCString(ctx, value_c_str);

    auto js_error = create_js_error(ex.what(), ctx);
    return JS_Throw(ctx, js_error);
  }
}

JSValue api_kv_get(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  auto *context = (Context *)JS_GetContextOpaque(ctx);
  if (context == nullptr) {
    auto js_error = create_js_error("context is null", ctx);
    return JS_Throw(ctx, js_error);
  }

  const auto *key_c_str = JS_ToCString(ctx, argv[0]);

  try {
    JSValue return_value = JS_NULL;

    auto value_json = context->capacitor_interface->kv_api_get(key_c_str);

    if (value_json.empty()) {
      JS_FreeCString(ctx, key_c_str);
      return return_value;
    }

    auto value_str_obj = JS_NewString(ctx, value_json.c_str());

    return_value = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, return_value, "value", value_str_obj);

    JS_FreeCString(ctx, key_c_str);

    return return_value;
  } catch (std::exception &ex) {
    JS_FreeCString(ctx, key_c_str);

    auto js_error = create_js_error(ex.what(), ctx);
    return JS_Throw(ctx, js_error);
  }
}

JSValue api_kv_remove(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  auto *context = (Context *)JS_GetContextOpaque(ctx);
  if (context == nullptr) {
    auto js_error = create_js_error("context is null", ctx);
    return JS_Throw(ctx, js_error);
  }

  const auto *key_c_str = JS_ToCString(ctx, argv[0]);

  try {
    context->capacitor_interface->kv_api_remove(key_c_str);

    JS_FreeCString(ctx, key_c_str);

    return JS_UNDEFINED;
  } catch (std::exception &ex) {
    JS_FreeCString(ctx, key_c_str);

    auto js_error = create_js_error(ex.what(), ctx);
    return JS_Throw(ctx, js_error);
  }
}
