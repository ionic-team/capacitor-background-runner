#include "api_kv.h"

#include "context.h"

JSValue api_kv_set(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  JSValue ret_value = JS_UNDEFINED;

  const auto *key_c_str = JS_ToCString(ctx, argv[0]);
  const auto *value_c_str = JS_ToCString(ctx, argv[1]);

  auto *parent_ctx = (Context *)JS_GetContextOpaque(ctx);
  auto *env = parent_ctx->getJNIEnv();
    
  auto *kv = env->GetObjectField(parent_ctx->capacitor_api, parent_ctx->jni_classes->capacitor_api_kv_field);

  jclass j_kv_class = env->FindClass("io/ionic/android_js_engine/api/KV");
  jmethodID j_kv_set_method = env->GetMethodID(j_kv_class, "set", "(Ljava/lang/String;Ljava/lang/String;)V");

  jstring key_j_str = env->NewStringUTF(key_c_str);
  jstring value_j_str = env->NewStringUTF(value_c_str);

  env->CallVoidMethod(kv, j_kv_set_method, key_j_str, value_j_str);

  JS_FreeCString(ctx, key_c_str);
  JS_FreeCString(ctx, value_c_str);

  return ret_value;
}

JSValue api_kv_get(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  JSValue ret_value = JS_UNDEFINED;

  const auto *key_c_str = JS_ToCString(ctx, argv[0]);

  auto *parent_ctx = (Context *)JS_GetContextOpaque(ctx);
  auto *env = parent_ctx->getJNIEnv();

  auto *kv = env->GetObjectField(parent_ctx->capacitor_api, parent_ctx->jni_classes->capacitor_api_kv_field);

  jclass j_kv_class = env->FindClass("io/ionic/android_js_engine/api/KV");
  jmethodID j_kv_get_method = env->GetMethodID(j_kv_class, "get", "(Ljava/lang/String;)Ljava/lang/String;");

  jstring key_j_str = env->NewStringUTF(key_c_str);

  auto value_j_str = (jstring)env->CallObjectMethod(kv, j_kv_get_method, key_j_str);
  auto value_c_str = env->GetStringUTFChars(value_j_str, nullptr);

  ret_value = JS_NewString(ctx, value_c_str);

  env->ReleaseStringUTFChars(value_j_str, value_c_str);

  JS_FreeCString(ctx, key_c_str);

  return ret_value;
}

JSValue api_kv_remove(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  JSValue ret_value = JS_UNDEFINED;

  const auto *key_c_str = JS_ToCString(ctx, argv[0]);

  auto *parent_ctx = (Context *)JS_GetContextOpaque(ctx);
  auto *env = parent_ctx->getJNIEnv();

  auto *kv = env->GetObjectField(parent_ctx->capacitor_api, parent_ctx->jni_classes->capacitor_api_kv_field);

  jclass j_kv_class = env->FindClass("io/ionic/android_js_engine/api/KV");
  jmethodID j_kv_remove_method = env->GetMethodID(j_kv_class, "remove", "(Ljava/lang/String;)V");

  jstring key_j_str = env->NewStringUTF(key_c_str);

  env->CallVoidMethod(kv, j_kv_remove_method, key_j_str);

  JS_FreeCString(ctx, key_c_str);

  return ret_value;
}