#include "errors.hpp"

NativeInterfaceException::NativeInterfaceException(const char *error) { this->message = error; }

const char *NativeInterfaceException::what() const throw() { return this->message.c_str(); }

JSValue create_js_error(const char *error_message, JSContext *ctx) {
  auto error = JS_NewError(ctx);
  JS_SetPropertyStr(ctx, error, "message", JS_NewString(ctx, error_message));
  return error;
}

JSValue get_js_exception(JSContext *ctx) {
  auto exception = JS_GetException(ctx);
  if (!JS_IsException(exception) && !JS_IsError(ctx, exception)) {
    return JS_NULL;
  }

  return exception;
}

void reject_promise(JSContext *ctx, JSValue reject_func, JSValue reject_obj) {
  auto global_obj = JS_GetGlobalObject(ctx);
  JSValueConst reject_args[1];
  reject_args[0] = reject_obj;

  JS_Call(ctx, reject_func, global_obj, 1, reject_args);
  JS_FreeValue(ctx, global_obj);
}