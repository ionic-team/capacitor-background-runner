#include <jni.h>
#include <string.h>
#include <android/log.h>
#include "quickjs/quickjs.h"
#include "quickjs/cutils.h"
#include "Context.h"
#include "api_console.h"
#include "api_events.h"

//static const JSCFunctionListEntry console_funcs[] = {
//        JS_CFUNC_DEF("log", 1, api_console_log),
//};

jint throw_js_exception(JNIEnv *env, JSContext* ctx) {
    jclass c = (*env).FindClass("io/ionic/backgroundrunner/android_engine/EngineErrors$JavaScriptException");

    JSValue err_message = JS_GetPropertyStr(ctx, JS_GetException(ctx), "message");
    const char* err_str = JS_ToCString(ctx, err_message);

    auto ret = (*env).ThrowNew(c, err_str);
    JS_FreeCString(ctx, err_str);

    return ret;
}

jobject js_value_to_java_object(JNIEnv *env, JSContext* ctx, JSValue value) {
    jclass c = (*env).FindClass("io/ionic/backgroundrunner/android_engine/JSValue");
    jmethodID constr = (*env).GetMethodID(c, "<init>", "()V");

    jobject obj = (*env).NewObject(c, constr);

    if(JS_IsBool(value)) {
        jclass b = (*env).FindClass("java/lang/Boolean");
        jmethodID b_constr = (*env).GetMethodID(b, "<init>", "(Z)V");

        jobject valueObj = (*env).NewObject(b, b_constr, JS_VALUE_GET_BOOL(value));

        jfieldID valueField = (*env).GetFieldID(c, "boolValue", "Ljava/lang/Boolean;");
        jfieldID flagField = (*env).GetFieldID(c, "isBool", "Z");

        (*env).SetObjectField(obj, valueField, valueObj);
        (*env).SetBooleanField(obj, flagField, true);

        return obj;
    }

    if (JS_IsNumber(value)) {
        jobject valueObj;

        jclass d = (*env).FindClass("java/lang/Double");
        jmethodID d_constr = (*env).GetMethodID(d, "<init>", "(D)V");
        if (JS_VALUE_GET_TAG(value) == JS_TAG_INT) {
            valueObj = (*env).NewObject(d, d_constr, (double)JS_VALUE_GET_INT(value));
        } else {
            valueObj = (*env).NewObject(d, d_constr, (double)JS_VALUE_GET_FLOAT64(value));
        }

        jfieldID valueField = (*env).GetFieldID(c, "doubleValue", "Ljava/lang/Double;");
        jfieldID flagField = (*env).GetFieldID(c, "isNumber", "Z");

        (*env).SetObjectField(obj, valueField, valueObj);
        (*env).SetBooleanField(obj, flagField, true);

        return obj;
    }

    if (JS_IsArray(ctx, value)) {
        jfieldID flagField = (*env).GetFieldID(c, "isArray", "Z");
        (*env).SetBooleanField(obj, flagField, true);

        return obj;
    }

    if(JS_IsString(value)) {
        jclass s = (*env).FindClass("java/lang/String");
        jmethodID s_constr = (*env).GetMethodID(s, "<init>", "(Ljava/lang/String;)V");

        const char * str = JS_ToCString(ctx, value);

        jstring j_str = (*env).NewStringUTF(str);
        jobject valueObj = (*env).NewObject(s, s_constr, j_str);

        jfieldID valueField = (*env).GetFieldID(c, "strValue", "Ljava/lang/String;");
        jfieldID flagField = (*env).GetFieldID(c, "isString", "Z");

        (*env).SetObjectField(obj, valueField, valueObj);
        (*env).SetBooleanField(obj, flagField, true);

        JS_FreeCString(ctx, str);

        return obj;
    }

    if (JS_IsUndefined(value)) {
        jfieldID flagField = (*env).GetFieldID(c, "isUndefined", "Z");
        (*env).SetBooleanField(obj, flagField, true);

        return obj;
    }

    if (JS_IsNull(value)) {
        jfieldID flagField = (*env).GetFieldID(c, "isNull", "Z");
        (*env).SetBooleanField(obj, flagField, true);

        return obj;
    }

    return obj;
}

extern "C"
JNIEXPORT jlong JNICALL
Java_io_ionic_backgroundrunner_android_1engine_Runner_00024Companion_initRunner(JNIEnv *env, jobject thiz) {
    JSRuntime *rt = JS_NewRuntime();
    JS_SetCanBlock(rt, 1);
    JS_SetMaxStackSize(rt, 0);

    return (jlong)(long)rt;
}
extern "C"
JNIEXPORT void JNICALL
Java_io_ionic_backgroundrunner_android_1engine_Runner_00024Companion_destroyRunner(JNIEnv *env, jobject thiz, jlong ptr) {
    JSRuntime *rt = (JSRuntime *)ptr;
    JS_FreeRuntime(rt);
}

extern "C"
JNIEXPORT jlong JNICALL
Java_io_ionic_backgroundrunner_android_1engine_Context_00024Companion_initContext(JNIEnv *env, jobject thiz, jlong runner_ptr, jstring name) {
    JSRuntime *rt = (JSRuntime *)runner_ptr;

    Context* context = new Context(env->GetStringUTFChars(name, 0), rt, env);
    return (jlong)(long)context;
}

extern "C"
JNIEXPORT void JNICALL
Java_io_ionic_backgroundrunner_android_1engine_Context_00024Companion_destroyContext(JNIEnv *env, jobject thiz, jlong ptr) {
    Context *ctx = (Context *)ptr;
    delete ctx;
}
extern "C"
JNIEXPORT jobject JNICALL
Java_io_ionic_backgroundrunner_android_1engine_Context_00024Companion_evaluate(JNIEnv *env, jobject thiz, jlong ptr, jstring code) {
    Context *ctx = (Context *)ptr;

    const char *c_code = env->GetStringUTFChars(code, 0);

    JSValue value = ctx->evaluate(c_code);

    if (JS_IsException(value)) {
        throw_js_exception(env, ctx->ctx);
        JS_FreeValue(ctx->ctx, value);
        return nullptr;
    }

    jobject obj = js_value_to_java_object(env, ctx->ctx, value);

    JS_FreeValue(ctx->ctx, value);

    return obj;
}

extern "C"
JNIEXPORT void JNICALL
Java_io_ionic_backgroundrunner_android_1engine_Context_00024Companion_dispatchEvent(JNIEnv *env, jobject thiz, jlong ptr, jstring event, jstring details) {
    Context *ctx = (Context *)ptr;

    const char *c_event = env->GetStringUTFChars(event, 0);
    const char *c_details_json = env->GetStringUTFChars(details, 0);

    JSValue details_obj = ctx->parseJSON(c_details_json);

    JSValue value = ctx->dispatch_event(c_event, details_obj);
    JS_FreeValue(ctx->ctx, details_obj);

    env->ReleaseStringUTFChars(event, c_event);
    env->ReleaseStringUTFChars(details, c_details_json);

    if (JS_IsException(value)) {
        throw_js_exception(env, ctx->ctx);
    }

    JS_FreeValue(ctx->ctx, value);

}