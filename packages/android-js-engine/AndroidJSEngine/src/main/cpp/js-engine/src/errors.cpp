#include "errors.hpp"

JSValue throw_js_exception(JSContext *ctx, const char *message) {
  auto exception = JS_NewError(ctx);
  JS_SetPropertyStr(ctx, exception, "message", JS_NewString(ctx, message));

  return JS_Throw(ctx, exception);
}

void reject_promise(JSContext *ctx, JSValue reject_func, JSValue reject_obj) {
  auto global_obj = JS_GetGlobalObject(ctx);
  JSValueConst reject_args[1];
  reject_args[0] = reject_obj;

  JS_Call(ctx, reject_func, global_obj, 1, reject_args);
  JS_FreeValue(ctx, global_obj);
}