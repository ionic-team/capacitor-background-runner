#include "api_timeout.h"

#include "Context.h"

JSValue create_timer(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv, bool repeat) {
  JSValue ret_value = JS_UNDEFINED;

  int timeout;

  if (!JS_IsNumber(argv[1])) {
    return JS_EXCEPTION;
  }

  if (!JS_IsFunction(ctx, argv[0])) {
    return JS_EXCEPTION;
  }

  timeout = JS_VALUE_GET_INT(argv[1]);

  // TODO: Check for JNI Exceptions
  Context *parent_ctx = (Context *)JS_GetContextOpaque(ctx);

  jclass j_context_class = parent_ctx->env->FindClass("io/ionic/backgroundrunner/Context");
  jmethodID j_method = parent_ctx->env->GetStaticMethodID(j_context_class, "randomHashCode", "()I");

  int unique = parent_ctx->env->CallStaticIntMethod(j_context_class, j_method);

  Timer timer{};
  timer.js_func = JS_DupValue(ctx, argv[0]);
  ;
  timer.timeout = timeout;
  timer.start = std::chrono::system_clock::now();
  timer.repeat = repeat;

  parent_ctx->timers_mutex.lock();

  parent_ctx->timers[unique] = timer;

  parent_ctx->timers_mutex.unlock();

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

  int id = JS_VALUE_GET_INT(argv[0]);

  // TODO: Check for JNI Exceptions
  Context *parent_ctx = (Context *)JS_GetContextOpaque(ctx);

  parent_ctx->timers_mutex.lock();

  JS_FreeValue(ctx, parent_ctx->timers[id].js_func);
  parent_ctx->timers.erase(id);

  parent_ctx->timers_mutex.unlock();

  return ret_value;
}
