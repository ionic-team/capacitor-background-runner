#include "api_kv.h"

#include "context.h"
#include "errors.h"

JSValue api_kv_set(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  JSValue ret_value = JS_UNDEFINED;
  JSValue jni_exception;

  const auto *key_c_str = JS_ToCString(ctx, argv[0]);
  const auto *value_c_str = JS_ToCString(ctx, argv[1]);

  auto *parent_ctx = (Context *)JS_GetContextOpaque(ctx);
  auto *env = parent_ctx->getJNIEnv();
    
  auto *kv = env->GetObjectField(parent_ctx->capacitor_api, parent_ctx->jni_classes->capacitor_api_kv_field);

  jmethodID j_kv_set_method = env->GetMethodID(parent_ctx->jni_classes->kv_api_class, "set", "(Ljava/lang/String;Ljava/lang/String;)V");
  jni_exception = check_and_throw_jni_exception(env, ctx);
  if (JS_IsException(jni_exception)) {
    return jni_exception;
  }

  jstring key_j_str = env->NewStringUTF(key_c_str);
  jstring value_j_str = env->NewStringUTF(value_c_str);

  env->CallVoidMethod(kv, j_kv_set_method, key_j_str, value_j_str);

  JS_FreeCString(ctx, key_c_str);
  JS_FreeCString(ctx, value_c_str);

  return ret_value;
}

JSValue api_kv_get(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  JSValue ret_value = JS_UNDEFINED;
  JSValue jni_exception;

  const auto *key_c_str = JS_ToCString(ctx, argv[0]);

  auto *parent_ctx = (Context *)JS_GetContextOpaque(ctx);
  auto *env = parent_ctx->getJNIEnv();

  auto *kv = env->GetObjectField(parent_ctx->capacitor_api, parent_ctx->jni_classes->capacitor_api_kv_field);

  jmethodID j_kv_get_method = env->GetMethodID(parent_ctx->jni_classes->kv_api_class, "get", "(Ljava/lang/String;)Ljava/lang/String;");
  jni_exception = check_and_throw_jni_exception(env, ctx);

  if (JS_IsException(jni_exception)) {
    JS_FreeCString(ctx, key_c_str);
    return jni_exception;
  }

  jstring key_j_str = env->NewStringUTF(key_c_str);

  auto value_j_str = (jstring)env->CallObjectMethod(kv, j_kv_get_method, key_j_str);
  jni_exception = check_and_throw_jni_exception(env, ctx);

  if (JS_IsException(jni_exception)) {
      JS_FreeCString(ctx, key_c_str);
    return jni_exception;
  }

  if (value_j_str == nullptr) {
      JS_FreeCString(ctx, key_c_str);
      return JS_NULL;
  }

  auto value_c_str = env->GetStringUTFChars(value_j_str, nullptr);

  ret_value = JS_NewString(ctx, value_c_str);

  env->ReleaseStringUTFChars(value_j_str, value_c_str);

  JS_FreeCString(ctx, key_c_str);

  return ret_value;
}

JSValue api_kv_remove(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  JSValue ret_value = JS_UNDEFINED;
  JSValue jni_exception;

  const auto *key_c_str = JS_ToCString(ctx, argv[0]);

  auto *parent_ctx = (Context *)JS_GetContextOpaque(ctx);
  auto *env = parent_ctx->getJNIEnv();

  auto *kv = env->GetObjectField(parent_ctx->capacitor_api, parent_ctx->jni_classes->capacitor_api_kv_field);


  jmethodID j_kv_remove_method = env->GetMethodID(parent_ctx->jni_classes->kv_api_class, "remove", "(Ljava/lang/String;)V");
  jni_exception = check_and_throw_jni_exception(env, ctx);

  if (JS_IsException(jni_exception)) {
    JS_FreeCString(ctx, key_c_str);
    return jni_exception;
  }

  jstring key_j_str = env->NewStringUTF(key_c_str);

  env->CallVoidMethod(kv, j_kv_remove_method, key_j_str);

  JS_FreeCString(ctx, key_c_str);

  return ret_value;
}
