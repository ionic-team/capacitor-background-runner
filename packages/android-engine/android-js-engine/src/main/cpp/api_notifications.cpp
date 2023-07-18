#include "api_notifications.h"

#include "context.h"
#include "errors.h"

JSValue api_notifications_schedule(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  JSValue jni_exception = JS_UNDEFINED;

  auto *parent_ctx = (Context *)JS_GetContextOpaque(ctx);
  auto *env = parent_ctx->getJNIEnv();

  auto options_json = parent_ctx->stringifyJSON(argv[0]);

  auto *notifications = env->GetObjectField(parent_ctx->capacitor_api, parent_ctx->jni_classes->capacitor_api_notification_field);
  jmethodID j_notification_schedule_method = env->GetMethodID(parent_ctx->jni_classes->notification_api_class, "schedule", "(Ljava/lang/String;)V");

  jstring j_options_json = env->NewStringUTF(options_json.c_str());

  env->CallVoidMethod(notifications, j_notification_schedule_method, j_options_json);
  jni_exception = check_and_throw_jni_exception(env, ctx);

  if (JS_IsException(jni_exception)) {
    return jni_exception;
  }

  return JS_UNDEFINED;
}