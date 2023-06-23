#include "api_kv.h"

#include "Context.h"

JSValue api_kv_set(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  JSValue ret_value = JS_UNDEFINED;

  auto key_c_str = JS_ToCString(ctx, argv[0]);
  auto value_c_str = JS_ToCString(ctx, argv[1]);

  Context *parent_ctx = (Context *)JS_GetContextOpaque(ctx);

  jclass j_context_class = parent_ctx->env->FindClass("io/ionic/backgroundrunner/Context");
  jmethodID j_method = parent_ctx->env->GetStaticMethodID(j_context_class, "KV_Set", "(Ljava/lang/String;Ljava/lang/String;)V");

  jstring key_j_str = parent_ctx->env->NewStringUTF(key_c_str);
  jstring value_j_str = parent_ctx->env->NewStringUTF(value_c_str);

  parent_ctx->env->CallStaticVoidMethod(j_context_class, j_method, key_j_str, value_j_str);

  JS_FreeCString(ctx, key_c_str);
  JS_FreeCString(ctx, value_c_str);

  return ret_value;
}

JSValue api_kv_get(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  JSValue ret_value = JS_UNDEFINED;

  auto key_c_str = JS_ToCString(ctx, argv[0]);

  Context *parent_ctx = (Context *)JS_GetContextOpaque(ctx);

  jclass j_context_class = parent_ctx->env->FindClass("io/ionic/backgroundrunner/Context");
  jmethodID j_method = parent_ctx->env->GetStaticMethodID(j_context_class, "KV_Get", "(Ljava/lang/String;)Ljava/lang/String;");

  jstring key_j_str = parent_ctx->env->NewStringUTF(key_c_str);

  jstring value_j_str = (jstring)parent_ctx->env->CallStaticObjectMethod(j_context_class, j_method, key_j_str);

  auto value_c_str = parent_ctx->env->GetStringUTFChars(value_j_str, 0);

  ret_value = JS_NewString(ctx, value_c_str);

  parent_ctx->env->ReleaseStringUTFChars(value_j_str, value_c_str);

  JS_FreeCString(ctx, key_c_str);

  return ret_value;
}

JSValue api_kv_remove(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  JSValue ret_value = JS_UNDEFINED;

  auto key_c_str = JS_ToCString(ctx, argv[0]);

  Context *parent_ctx = (Context *)JS_GetContextOpaque(ctx);

  jclass j_context_class = parent_ctx->env->FindClass("io/ionic/backgroundrunner/Context");
  jmethodID j_method = parent_ctx->env->GetStaticMethodID(j_context_class, "KV_Remove", "(Ljava/lang/String;)V");

  jstring key_j_str = parent_ctx->env->NewStringUTF(key_c_str);

  parent_ctx->env->CallStaticVoidMethod(j_context_class, j_method, key_j_str);

  JS_FreeCString(ctx, key_c_str);

  return ret_value;
}
