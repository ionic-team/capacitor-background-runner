#include "errors.h"

bool throw_js_exception_in_jvm(JNIEnv *env, JSContext *ctx, JSValue value) {
    if (JS_IsException(value)) {
        JS_FreeValue(ctx, value);

        jclass error_class = env->FindClass("io/ionic/android_js_engine/EngineErrors$JavaScriptException");

        JSValue exception = JS_GetException(ctx);

        JSValue err_message = JS_GetPropertyStr(ctx, exception, "message");
        const char *err_message_c_str = JS_ToCString(ctx, err_message);

        env->ThrowNew(error_class, err_message_c_str);

        JS_FreeCString(ctx, err_message_c_str);

        JS_FreeValue(ctx, err_message);
        JS_FreeValue(ctx, exception);

        return true;
    }

    return false;
}

bool throw_jvm_exception_in_js(JNIEnv *env, JSContext *ctx) {
    if (!env->ExceptionCheck()) {
        return JS_NULL;
    }

    auto throwable = env->ExceptionOccurred();
    env->ExceptionClear();

    jclass exception_class = env->FindClass("java/lang/Exception");
    jmethodID exception_constructor = env->GetMethodID(exception_class, "<init>", "(Ljava/lang/Throwable;)V");
    jmethodID get_message_method = env->GetMethodID(exception_class, "getMessage", "()Ljava/lang/String;");

    auto err_obj = env->NewObject(exception_class, exception_constructor, throwable);
    auto err_msg = (jstring)env->CallObjectMethod(err_obj, get_message_method);

    auto c_errMsg = env->GetStringUTFChars(err_msg, nullptr);

    JSValue exception_value = JS_NewError(ctx);
    JS_SetPropertyStr(ctx, exception_value, "message", JS_NewString(ctx, c_errMsg));

    env->ReleaseStringUTFChars(err_msg, c_errMsg);

    return JS_Throw(ctx, exception_value);
}
