#include "api_console.h"

#include "context.h"

void write_to_logcat(android_LogPriority priority, const char *tag, const char *message) { __android_log_write(priority, tag, message); }

JSValue api_console_log(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  int i;
  const char *str;
  size_t len;

  auto *parent_ctx = (Context *)JS_GetContextOpaque(ctx);
  std::string tag = "[Runner Context " + parent_ctx->name + "]";

  for (i = 0; i < argc; i++) {
    str = JS_ToCStringLen(ctx, &len, argv[i]);
    if (!str) {
      return JS_EXCEPTION;
    }

    write_to_logcat(ANDROID_LOG_INFO, tag.c_str(), str);

    JS_FreeCString(ctx, str);
  }

  return JS_UNDEFINED;
};

JSValue api_console_warn(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  int i;
  const char *str;
  size_t len;

  auto *parent_ctx = (Context *)JS_GetContextOpaque(ctx);
  std::string tag = "[Runner Context " + parent_ctx->name + "]";

  for (i = 0; i < argc; i++) {
    str = JS_ToCStringLen(ctx, &len, argv[i]);
    if (!str) {
      return JS_EXCEPTION;
    }

    write_to_logcat(ANDROID_LOG_WARN, tag.c_str(), str);

    JS_FreeCString(ctx, str);
  }

  return JS_UNDEFINED;
};

JSValue api_console_error(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  int i;
  const char *str;
  size_t len;

  auto *parent_ctx = (Context *)JS_GetContextOpaque(ctx);
  std::string tag = "[Runner Context " + parent_ctx->name + "]";

  for (i = 0; i < argc; i++) {
    str = JS_ToCStringLen(ctx, &len, argv[i]);
    if (!str) {
      return JS_EXCEPTION;
    }

    write_to_logcat(ANDROID_LOG_ERROR, tag.c_str(), str);

    JS_FreeCString(ctx, str);
  }

  return JS_UNDEFINED;
};

JSValue api_console_debug(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  int i;
  const char *str;
  size_t len;

  auto *parent_ctx = (Context *)JS_GetContextOpaque(ctx);
  std::string tag = "[Runner Context " + parent_ctx->name + "]";

  for (i = 0; i < argc; i++) {
    str = JS_ToCStringLen(ctx, &len, argv[i]);
    if (!str) {
      return JS_EXCEPTION;
    }

    write_to_logcat(ANDROID_LOG_DEBUG, tag.c_str(), str);

    JS_FreeCString(ctx, str);
  }

  return JS_UNDEFINED;
}
