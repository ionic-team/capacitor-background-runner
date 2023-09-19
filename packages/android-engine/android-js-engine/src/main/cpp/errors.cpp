#include "errors.h"

bool throw_js_exception_in_jvm(JNIEnv *env, JSContext *ctx, JSValue value) {
    if (JS_IsException(value)) {
        JS_FreeValue(ctx, value);

        jclass error_class = env->FindClass("io/ionic/android_js_engine/EngineErrors$JavaScriptException");

        JSValue const exception = JS_GetException(ctx);

        JSValue const err_message = JS_GetPropertyStr(ctx, exception, "message");
        const char *err_message_c_str = JS_ToCString(ctx, err_message);

        env->ThrowNew(error_class, err_message_c_str);

        JS_FreeCString(ctx, err_message_c_str);

        JS_FreeValue(ctx, err_message);
        JS_FreeValue(ctx, exception);

        return true;
    }

    return false;
}

JSValue throw_jvm_exception_in_js(JNIEnv *env, JSContext *ctx) {
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

    const auto *c_err_msg = env->GetStringUTFChars(err_msg, nullptr);

    auto exception = throw_js_exception(ctx, c_err_msg);

    env->ReleaseStringUTFChars(err_msg, c_err_msg);

    return exception;
}

JSValue throw_js_exception(JSContext *ctx, const char * message) {
    auto exception = JS_NewError(ctx);
    JS_SetPropertyStr(ctx, exception, "message", JS_NewString(ctx, message));

    return JS_Throw(ctx, exception);
}

void reject_promise(JSContext *ctx, JSValue reject_func, JSValue reject_obj) {
    auto global_obj = JS_GetGlobalObject(ctx);
    JSValueConst reject_args[1];
    reject_args[0] = reject_obj;

    JS_Call(ctx, reject_func, global_obj, 1,  reject_args);
    JS_FreeValue(ctx, global_obj);
}
