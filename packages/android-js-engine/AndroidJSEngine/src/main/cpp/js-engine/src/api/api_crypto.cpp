#include "api_crypto.h"

#include "../context.hpp"
#include "../errors.hpp"

JSValue api_crypto_get_random_values(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  uint8_t *buf;
  size_t elem, len, offset, buf_size;
  int size;

  auto t_arr_buf = JS_GetTypedArrayBuffer(ctx, argv[0], &offset, &len, &elem);
  buf = JS_GetArrayBuffer(ctx, &buf_size, t_arr_buf);

  if (!buf) {
    return JS_EXCEPTION;
  }

  size = buf_size;

  auto *context = (Context *)JS_GetContextOpaque(ctx);
  if (context == nullptr) {
    auto js_error = create_js_error("parent context is null", ctx);
    return JS_Throw(ctx, js_error);
  }

  try {
    auto random_bytes = context->native_interface->crypto_get_random(size);

    for (int i = 0; i < size; i++) {
      buf[i] = random_bytes[i];
    }

    JS_FreeValue(ctx, t_arr_buf);

    return JS_DupValue(ctx, argv[0]);
  } catch (std::exception &ex) {
    JS_FreeValue(ctx, t_arr_buf);

    auto js_error = create_js_error(ex.what(), ctx);
    return JS_Throw(ctx, js_error);
  }
}

JSValue api_crypto_random_uuid(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  auto *context = (Context *)JS_GetContextOpaque(ctx);
  if (context == nullptr) {
    auto js_error = create_js_error("parent context is null", ctx);
    return JS_Throw(ctx, js_error);
  }

  try {
    auto uuid_string = context->native_interface->crypto_get_random_uuid();
    auto ret_value = JS_NewString(ctx, uuid_string.c_str());
    return ret_value;
  } catch (std::exception &ex) {
    auto js_error = create_js_error(ex.what(), ctx);
    return JS_Throw(ctx, js_error);
  }
}