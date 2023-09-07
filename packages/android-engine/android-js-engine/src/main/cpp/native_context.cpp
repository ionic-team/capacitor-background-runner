#include <jni.h>
#include "runner.h"
#include "context.h"
#include "errors.h"

extern "C"
JNIEXPORT jlong JNICALL
Java_io_ionic_android_1js_1engine_Context_createRunnerContext(JNIEnv *env, jobject thiz, jlong runner_ptr, jstring name) {
    auto *runner = (Runner *)runner_ptr;
    return (jlong)runner->create_context(env->GetStringUTFChars(name, nullptr), env);
}
extern "C"
JNIEXPORT void JNICALL
Java_io_ionic_android_1js_1engine_Context_destroyRunnerContext(JNIEnv *env, jobject thiz, jlong runner_ptr, jlong ptr) {
    auto *runner = (Runner *)runner_ptr;
    auto *ctx = (Context *)ptr;

    runner->destroy_context(ctx->name);
}
extern "C"
JNIEXPORT jstring JNICALL
Java_io_ionic_android_1js_1engine_Context_evaluate(JNIEnv *env, jobject thiz, jlong ptr, jstring code, jboolean ret_value) {
    auto *ctx = (Context *)ptr;

    const char *c_code = env->GetStringUTFChars(code, nullptr);
    auto value = ctx->evaluate(c_code, (bool)ret_value);

    if (throw_js_exception_in_jvm(env, ctx->qjs_context, value)) {
        JS_FreeValue(ctx->qjs_context, value);
        return nullptr;
    }

    const char *json_string = JS_ToCString(ctx->qjs_context, value);
    auto *j_json_string = env->NewStringUTF(json_string);

    JS_FreeCString(ctx->qjs_context, json_string);
    JS_FreeValue(ctx->qjs_context, value);

    return j_json_string;
}
extern "C"
JNIEXPORT void JNICALL
Java_io_ionic_android_1js_1engine_Context_registerGlobalFunction(JNIEnv *env, jobject thiz, jlong ptr, jstring function_name, jobject function) {
    auto *context = (Context *)ptr;

    const auto *c_function_name = env->GetStringUTFChars(function_name, nullptr);
    context->register_function(c_function_name, env->NewGlobalRef(function));

    env->ReleaseStringUTFChars(function_name, c_function_name);
}
extern "C"
JNIEXPORT void JNICALL
Java_io_ionic_android_1js_1engine_Context_dispatchEvent(JNIEnv *env, jobject thiz, jlong ptr, jstring event, jstring details) {
    auto *context = (Context *)ptr;

    const char *c_event = env->GetStringUTFChars(event, nullptr);
    const char *c_details_json = env->GetStringUTFChars(details, nullptr);

    auto details_obj = JS_ParseJSON(context->qjs_context, c_details_json, strlen(c_details_json), "<details>");

    auto value = context->dispatch_event(c_event, details_obj);
    JS_FreeValue(context->qjs_context, details_obj);

    env->ReleaseStringUTFChars(event, c_event);
    env->ReleaseStringUTFChars(details, c_details_json);

    throw_js_exception_in_jvm(env, context->qjs_context, value);

    JS_FreeValue(context->qjs_context, value);
}