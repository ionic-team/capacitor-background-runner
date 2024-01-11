#include "errors.h"

JSValue create_js_error(const char * error_message, JSContext *ctx) {
    auto error = JS_NewError(ctx);
    JS_SetPropertyStr(ctx, error, "message", JS_NewString(ctx, error_message));
    return error;
}

JSValue get_jvm_exception(JNIEnv *env, JSContext *ctx) {
    if (!env->ExceptionCheck()) {
        return JS_NULL;
    }

    auto *throwable = env->ExceptionOccurred();
    env->ExceptionClear();

    jclass exception_class = env->FindClass("java/lang/Exception");
    jmethodID exception_constructor = env->GetMethodID(exception_class, "<init>", "(Ljava/lang/Throwable;)V");
    jmethodID get_message_method = env->GetMethodID(exception_class, "getMessage", "()Ljava/lang/String;");

    auto *err_obj = env->NewObject(exception_class, exception_constructor, throwable);
    auto *err_msg = (jstring)env->CallObjectMethod(err_obj, get_message_method);

    const auto *err_message_c_str = env->GetStringUTFChars(err_msg, nullptr);

    return create_js_error(err_message_c_str, ctx);
}

JSValue get_js_exception(JSContext *ctx) {
    auto exception = JS_GetException(ctx);
    if (!JS_IsException(exception)  && !JS_IsError(ctx, exception)) {
        return JS_NULL;
    }

    return exception;
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


