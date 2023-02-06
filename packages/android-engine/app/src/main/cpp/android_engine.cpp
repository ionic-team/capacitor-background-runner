#include <jni.h>
#include <string.h>
#include "quickjs/quickjs.h"

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
Java_io_ionic_backgroundrunner_android_1engine_Context_00024Companion_initContext(JNIEnv *env, jobject thiz, jlong runner_ptr) {
    JSRuntime *rt = (JSRuntime *)runner_ptr;
    JSContext *ctx = JS_NewContext(rt);

    return (jlong)(long)ctx;
}
extern "C"
JNIEXPORT void JNICALL
Java_io_ionic_backgroundrunner_android_1engine_Context_00024Companion_destroyContext(JNIEnv *env, jobject thiz, jlong ptr) {
    JSContext *ctx = (JSContext *)ptr;
    JS_FreeContext(ctx);
}
extern "C"
JNIEXPORT jobject JNICALL
Java_io_ionic_backgroundrunner_android_1engine_Context_00024Companion_evaluate(JNIEnv *env, jobject thiz, jlong ptr, jstring code) {
    JSContext *ctx = (JSContext *)ptr;

    int flags = JS_EVAL_TYPE_GLOBAL;
    flags |= JS_EVAL_FLAG_BACKTRACE_BARRIER;

    const char *c_code = env->GetStringUTFChars(code, 0);

    JSValue value = JS_Eval(ctx, c_code, strlen(c_code), "<code>", flags);

    jobject obj = js_value_to_java_object(env, ctx, value);

    JS_FreeValue(ctx, value);

    return obj;
}