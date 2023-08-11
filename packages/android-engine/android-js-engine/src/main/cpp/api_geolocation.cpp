#include "api_geolocation.h"

#include "context.h"
#include "errors.h"

JSValue api_geolocation_current_location(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  JSValue ret_value, jni_exception = JS_UNDEFINED;

  auto *parent_ctx = (Context *)JS_GetContextOpaque(ctx);
  auto *env = parent_ctx->getJNIEnv();

  auto *geolocation = env->GetObjectField(parent_ctx->capacitor_api, parent_ctx->jni_classes->capacitor_api_geolocation_field);

  jmethodID j_geolocation_current_location_method = env->GetMethodID(parent_ctx->jni_classes->geolocation_api_class, "getCurrentPosition", "()Ljava/lang/String;");

  auto value_j_str = (jstring)env->CallObjectMethod(geolocation, j_geolocation_current_location_method);
  jni_exception = check_and_throw_jni_exception(env, ctx);
  if (JS_IsException(jni_exception)) {
    return jni_exception;
  }

  if (value_j_str == nullptr) {
    return JS_NULL;
  }

  auto value_c_str = env->GetStringUTFChars(value_j_str, nullptr);

  ret_value = parent_ctx->parseJSON(value_c_str);

  env->ReleaseStringUTFChars(value_j_str, value_c_str);

  return ret_value;
}
