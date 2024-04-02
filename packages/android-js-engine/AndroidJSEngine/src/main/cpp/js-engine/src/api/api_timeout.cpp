#include "api_timeout.h"

#include "../context.hpp"

JSValue create_timer(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv, bool repeat) {
  JSValue ret_value = JS_UNDEFINED;

  if (!JS_IsNumber(argv[1])) {
    return JS_EXCEPTION;
  }

  if (!JS_IsFunction(ctx, argv[0])) {
    return JS_EXCEPTION;
  }

  int const timeout = JS_VALUE_GET_INT(argv[1]);

  auto *context = (Context *)JS_GetContextOpaque(ctx);
  if (context == nullptr) {
    auto js_error = create_js_error("context is null", ctx);
    return JS_Throw(ctx, js_error);
  }

  try {
    int const unique = context->native_interface->get_random_hash();

    Timer timer{};
    timer.js_func = JS_DupValue(ctx, argv[0]);

    timer.timeout = timeout;
    timer.start = std::chrono::system_clock::now();
    timer.repeat = repeat;

    context->timers[unique] = timer;

    ret_value = JS_NewInt32(ctx, unique);

    return ret_value;
  } catch (std::exception &ex) {
    auto js_error = create_js_error(ex.what(), ctx);
    return JS_Throw(ctx, js_error);
  }
}

JSValue api_set_timeout(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) { return create_timer(ctx, this_val, argc, argv, false); }

JSValue api_set_interval(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) { return create_timer(ctx, this_val, argc, argv, true); }

JSValue api_clear_timeout(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  JSValue ret_value = JS_UNDEFINED;

  if (!JS_IsNumber(argv[0])) {
    return JS_EXCEPTION;
  }

  int const id = JS_VALUE_GET_INT(argv[0]);

  auto *context = (Context *)JS_GetContextOpaque(ctx);
  if (context == nullptr) {
    auto js_error = create_js_error("context is null", ctx);
    return JS_Throw(ctx, js_error);
  }

  JS_FreeValue(ctx, context->timers[id].js_func);
  context->timers.erase(id);

  return ret_value;
}
