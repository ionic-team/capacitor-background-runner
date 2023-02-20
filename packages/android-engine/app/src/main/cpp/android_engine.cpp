#include <jni.h>
#include <string.h>
#include <android/log.h>
#include "quickjs/quickjs.h"
#include "quickjs/cutils.h"
#include "Context.h"
#include "api_console.h"
#include "api_events.h"


jint throw_js_exception(JNIEnv *env, JSContext* ctx) {
    jclass c = (*env).FindClass("io/ionic/backgroundrunner/android_engine/EngineErrors$JavaScriptException");

    JSValue err_message = JS_GetPropertyStr(ctx, JS_GetException(ctx), "message");
    const char* err_str = JS_ToCString(ctx, err_message);

    auto ret = (*env).ThrowNew(c, err_str);
    JS_FreeCString(ctx, err_str);

    return ret;
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
JNIEXPORT jstring JNICALL
Java_io_ionic_backgroundrunner_android_1engine_Context_00024Companion_evaluate(JNIEnv *env, jobject thiz, jlong ptr, jstring code, jboolean ret_value) {
    Context *ctx = (Context *)ptr;

    const char *c_code = env->GetStringUTFChars(code, 0);

    auto json_string = ctx->evaluate(c_code, (bool)ret_value);

//    if (JS_IsException(value)) {
//        throw_js_exception(env, ctx->ctx);
//        JS_FreeValue(ctx->ctx, value);
//        return nullptr;
//    }

    const char * c_str_json = json_string.c_str();

    return env->NewStringUTF(c_str_json);
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

extern "C"
JNIEXPORT void JNICALL
Java_io_ionic_backgroundrunner_android_1engine_Context_00024Companion_start(JNIEnv *env ,jobject thiz, jlong ptr) {
    Context *ctx = (Context *)ptr;
    ctx->start_run_loop();
}
extern "C"
JNIEXPORT void JNICALL
Java_io_ionic_backgroundrunner_android_1engine_Context_00024Companion_stop(JNIEnv *env, jobject thiz, jlong ptr) {
    Context *ctx = (Context *)ptr;
    ctx->stop_run_loop();
}
extern "C"
JNIEXPORT void JNICALL
Java_io_ionic_backgroundrunner_android_1engine_Context_00024Companion_registerGlobalFunction(JNIEnv *env, jobject thiz, jlong ptr, jstring function_name, jobject function) {
    auto c_function_name = env->GetStringUTFChars(function_name, 0);
    
    Context *ctx = (Context *)ptr;
    ctx->register_function(c_function_name, env->NewGlobalRef(function));

    env->ReleaseStringUTFChars(function_name, c_function_name);
}