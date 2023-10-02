#include "api_cap_kv.h"

#include "../context.h"
#include "../errors.h"

JSValue api_kv_set(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  const auto *key_c_str = JS_ToCString(ctx, argv[0]);
  const auto *value_c_str = JS_ToCString(ctx, argv[1]);

  auto *context = (Context *)JS_GetContextOpaque(ctx);
  auto *env = context->java->getEnv();

  auto *kv = env->GetObjectField(context->cap_api, context->java->capacitor_api_kv_field);
  auto exception = throw_jvm_exception_in_js(env, ctx);
  if (JS_IsException(exception)) {
    JS_FreeCString(ctx, key_c_str);
    return exception;
  }

  jstring key_j_str = env->NewStringUTF(key_c_str);
  jstring value_j_str = env->NewStringUTF(value_c_str);

  env->CallVoidMethod(kv, context->java->capacitor_api_kv_set_method, key_j_str, value_j_str);

  JS_FreeCString(ctx, key_c_str);
  JS_FreeCString(ctx, value_c_str);

  return JS_UNDEFINED;
}

JSValue api_kv_get(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  const auto *key_c_str = JS_ToCString(ctx, argv[0]);

  auto *context = (Context *)JS_GetContextOpaque(ctx);
  auto *env = context->java->getEnv();

  auto *kv = env->GetObjectField(context->cap_api, context->java->capacitor_api_kv_field);

  jstring key_j_str = env->NewStringUTF(key_c_str);

  auto *value_j_str = (jstring)env->CallObjectMethod(kv, context->java->capacitor_api_kv_get_method, key_j_str);
  auto exception = throw_jvm_exception_in_js(env, ctx);
  if (JS_IsException(exception)) {
    JS_FreeCString(ctx, key_c_str);
    return exception;
  }

  if (value_j_str == nullptr) {
    JS_FreeCString(ctx, key_c_str);
    return JS_NULL;
  }

  const auto *value_c_str = env->GetStringUTFChars(value_j_str, nullptr);
  auto value_str_obj = JS_NewString(ctx, value_c_str);

  auto ret_value = JS_NewObject(ctx);
  JS_SetPropertyStr(ctx, ret_value, "value", value_str_obj);

  env->ReleaseStringUTFChars(value_j_str, value_c_str);
  JS_FreeCString(ctx, key_c_str);

  return ret_value;
}

JSValue api_kv_remove(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  const auto *key_c_str = JS_ToCString(ctx, argv[0]);

  auto *context = (Context *)JS_GetContextOpaque(ctx);
  auto *env = context->java->getEnv();

  auto *kv = env->GetObjectField(context->cap_api, context->java->capacitor_api_kv_field);

  jstring key_j_str = env->NewStringUTF(key_c_str);

  env->CallVoidMethod(kv, context->java->capacitor_api_kv_remove_method, key_j_str);
  JS_FreeCString(ctx, key_c_str);

  auto exception = throw_jvm_exception_in_js(env, ctx);
  if (JS_IsException(exception)) {
    return exception;
  }

  return JS_UNDEFINED;
}
