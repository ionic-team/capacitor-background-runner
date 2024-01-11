#include <jni.h>

#include "errors.h"
#include "./js-engine/src/runner.hpp"
#include "./js-engine/src/context.hpp"

extern "C"
JNIEXPORT jlong JNICALL
Java_io_ionic_android_1js_1engine_Context_createRunnerContext(JNIEnv *env, jobject thiz, jlong runner_ptr, jstring name) {
    auto *runner = (Runner *)runner_ptr;
    return (jlong)(long)runner->create_context(env->GetStringUTFChars(name, nullptr));
}
extern "C"
JNIEXPORT void JNICALL
Java_io_ionic_android_1js_1engine_Context_destroyRunnerContext(JNIEnv *env, jobject thiz, jlong runner_ptr, jlong ptr) {
    auto *runner = (Runner *)runner_ptr;
    auto *context = (Context *)ptr;

    runner->destroy_context(context->name);
}
extern "C"
JNIEXPORT jstring JNICALL
Java_io_ionic_android_1js_1engine_Context_evaluate(JNIEnv *env, jobject thiz, jlong ptr, jstring code, jboolean ret_value) {
    auto *context = (Context *)ptr;

    const char *code_c_str = env->GetStringUTFChars(code, nullptr);
    auto value = context->evaluate(code_c_str, (bool)ret_value);

    // check for thrown exception in js context
    auto exception = get_js_exception(context->qjs_context);
    if (throw_js_error_in_jvm(env, context->qjs_context, exception)) {
        return nullptr;
    }

    const char *json_c_str = JS_ToCString(context->qjs_context, value);
    auto *json_j_str = env->NewStringUTF(json_c_str);

    JS_FreeCString(context->qjs_context, json_c_str);
    JS_FreeValue(context->qjs_context, value);

    return json_j_str;
}
extern "C"
JNIEXPORT void JNICALL
Java_io_ionic_android_1js_1engine_Context_registerGlobalFunction(JNIEnv *env, jobject thiz, jlong ptr, jstring function_name, jobject function) {
    auto *context = (Context *)ptr;

    const auto *function_name_c_str = env->GetStringUTFChars(function_name, nullptr);

    std::any wrapped_function = env->NewGlobalRef(function);
    context->register_function(function_name_c_str, wrapped_function);

    env->ReleaseStringUTFChars(function_name, function_name_c_str);
}
extern "C"
JNIEXPORT void JNICALL
Java_io_ionic_android_1js_1engine_Context_dispatchEvent(JNIEnv *env, jobject thiz, jlong ptr, jstring event, jstring details) {
    auto *context = (Context *)ptr;

    const char *event_c_str = env->GetStringUTFChars(event, nullptr);
    const char *details_json_c_str = env->GetStringUTFChars(details, nullptr);

    auto details_obj = JS_ParseJSON(context->qjs_context, details_json_c_str, strlen(details_json_c_str), "<details>");

    auto value = context->dispatch_event(event_c_str, details_obj);
    JS_FreeValue(context->qjs_context, details_obj);

    env->ReleaseStringUTFChars(event, event_c_str);
    env->ReleaseStringUTFChars(details, details_json_c_str);

    // check for thrown exception in js context
    auto exception = get_js_exception(context->qjs_context);
    if (throw_js_error_in_jvm(env, context->qjs_context, exception)) {
        return;
    }

    JS_FreeValue(context->qjs_context, value);
}