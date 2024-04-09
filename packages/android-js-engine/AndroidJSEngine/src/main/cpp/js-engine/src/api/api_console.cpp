#include "api_console.h"

#include "../context.hpp"

JSValue api_console_log(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  int i;
  const char *str;
  size_t len;

  auto *parent_ctx = (Context *)JS_GetContextOpaque(ctx);
  if (parent_ctx == nullptr) {
    auto js_error = create_js_error("context is null", ctx);
    return JS_Throw(ctx, js_error);
  }

  std::string const tag = "Runner Context " + parent_ctx->name;

  for (i = 0; i < argc; i++) {
    str = JS_ToCStringLen(ctx, &len, argv[i]);
    if (!str) {
      return JS_EXCEPTION;
    }

    parent_ctx->native_interface->logger(INFO, tag, str);

    JS_FreeCString(ctx, str);
  }

  return JS_UNDEFINED;
};

JSValue api_console_warn(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  int i;
  const char *str;
  size_t len;

  auto *parent_ctx = (Context *)JS_GetContextOpaque(ctx);
  std::string const tag = "Runner Context " + parent_ctx->name;

  for (i = 0; i < argc; i++) {
    str = JS_ToCStringLen(ctx, &len, argv[i]);
    if (!str) {
      return JS_EXCEPTION;
    }

    parent_ctx->native_interface->logger(WARN, tag, str);

    JS_FreeCString(ctx, str);
  }

  return JS_UNDEFINED;
};

JSValue api_console_error(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  int i;
  const char *str;
  size_t len;

  auto *parent_ctx = (Context *)JS_GetContextOpaque(ctx);
  std::string const tag = "Runner Context " + parent_ctx->name;

  for (i = 0; i < argc; i++) {
    str = JS_ToCStringLen(ctx, &len, argv[i]);
    if (!str) {
      return JS_EXCEPTION;
    }

    parent_ctx->native_interface->logger(ERROR, tag, str);

    JS_FreeCString(ctx, str);
  }

  return JS_UNDEFINED;
};

JSValue api_console_debug(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  int i;
  const char *str;
  size_t len;

  auto *parent_ctx = (Context *)JS_GetContextOpaque(ctx);
  std::string const tag = "Runner Context " + parent_ctx->name;

  for (i = 0; i < argc; i++) {
    str = JS_ToCStringLen(ctx, &len, argv[i]);
    if (!str) {
      return JS_EXCEPTION;
    }

    parent_ctx->native_interface->logger(DEBUG, tag, str);

    JS_FreeCString(ctx, str);
  }

  return JS_UNDEFINED;
}