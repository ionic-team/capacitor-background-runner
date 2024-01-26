#import "java_errors.h"

NativeInterfaceException *get_jvm_exception(JNIEnv *env) {
  if (!env->ExceptionCheck()) {
    return nullptr;
  }

  auto *throwable = env->ExceptionOccurred();
  env->ExceptionClear();

  jclass exception_class = env->FindClass("java/lang/Exception");
  jmethodID exception_constructor = env->GetMethodID(exception_class, "<init>", "(Ljava/lang/Throwable;)V");
  jmethodID get_message_method = env->GetMethodID(exception_class, "getMessage", "()Ljava/lang/String;");

  auto *err_obj = env->NewObject(exception_class, exception_constructor, throwable);
  auto *err_msg = (jstring)env->CallObjectMethod(err_obj, get_message_method);

  const auto *err_message_c_str = env->GetStringUTFChars(err_msg, nullptr);

  return new NativeInterfaceException(err_message_c_str);
}

bool throw_js_error_in_jvm(JNIEnv *env, JSContext *ctx, JSValue value) {
  if (!JS_IsError(ctx, value) && !JS_IsException(value)) {
    return false;
  }

  jclass error_class = env->FindClass("io/ionic/android_js_engine/EngineErrors$JavaScriptException");

  JSValue const err_message = JS_GetPropertyStr(ctx, value, "message");
  const char *err_message_c_str = JS_ToCString(ctx, err_message);

  env->ThrowNew(error_class, err_message_c_str);

  JS_FreeCString(ctx, err_message_c_str);
  JS_FreeValue(ctx, err_message);
  JS_FreeValue(ctx, value);

  return true;
}
