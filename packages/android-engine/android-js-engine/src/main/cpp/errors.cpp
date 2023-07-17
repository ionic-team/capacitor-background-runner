#include "errors.h"

bool check_and_throw_js_exception(JNIEnv *env, JSContext *ctx, JSValue value) {
  if (JS_IsException(value)) {
    JS_FreeValue(ctx, value);

    jclass c = env->FindClass("io/ionic/android_js_engine/EngineErrors$JavaScriptException");

    JSValue exception = JS_GetException(ctx);

    JSValue err_message = JS_GetPropertyStr(ctx, exception, "message");
    const char *err_str = JS_ToCString(ctx, err_message);

    env->ThrowNew(c, err_str);

    JS_FreeCString(ctx, err_str);

    JS_FreeValue(ctx, err_message);
    JS_FreeValue(ctx, exception);

    return true;
  }

  return false;
}

JSValue check_and_throw_jni_exception(JNIEnv *env, JSContext *ctx) {
  if (!env->ExceptionCheck()) {
    return JS_NULL;
  }

  auto throwable = env->ExceptionOccurred();
  env->ExceptionClear();

  jclass c = env->FindClass("java/lang/Exception");
  jmethodID cnstrctr = env->GetMethodID(c, "<init>", "(Ljava/lang/Throwable;)V");
  jmethodID getMessage = env->GetMethodID(c, "getMessage", "()Ljava/lang/String;");

  auto errObj = env->NewObject(c, cnstrctr, throwable);
  auto errMsg = (jstring)env->CallObjectMethod(errObj, getMessage);

  auto c_errMsg = env->GetStringUTFChars(errMsg, nullptr);

  JSValue exceptionValue = JS_NewError(ctx);
  JS_SetPropertyStr(ctx, exceptionValue, "message", JS_NewString(ctx, c_errMsg));

  env->ReleaseStringUTFChars(errMsg, c_errMsg);

  return JS_Throw(ctx, exceptionValue);
}
