#include "api_timeout.h"

#include "../context.h"
#include "../errors.h"

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

  auto *env = context->java->getEnv();
  if (env == nullptr) {
    return throw_js_exception(ctx, "JVM Environment is null");
  }

  int const unique = env->CallStaticIntMethod(context->java->web_api_class, context->java->web_api_randomHashCode_method);
  auto exception = throw_jvm_exception_in_js(env, ctx);
  if (JS_IsException(exception)) {
    return exception;
  }

  Timer timer{};
  timer.js_func = JS_DupValue(ctx, argv[0]);

  timer.timeout = timeout;
  timer.start = std::chrono::system_clock::now();
  timer.repeat = repeat;

  context->timers[unique] = timer;

  ret_value = JS_NewInt32(ctx, unique);

  return ret_value;
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

  JS_FreeValue(ctx, context->timers[id].js_func);
  context->timers.erase(id);

  return ret_value;
}
