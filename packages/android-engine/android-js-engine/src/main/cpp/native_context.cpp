#include <jni.h>

#include "context.h"
#include "errors.h"
#include "quickjs/quickjs.h"
#include "runner.h"

extern "C" JNIEXPORT jlong JNICALL Java_io_ionic_android_1js_1engine_Context_initContext(JNIEnv *env, jobject thiz, jlong runner_ptr, jstring name) {
  auto *runner = (Runner *)runner_ptr;

  auto context_api_field_id = env->GetFieldID(env->FindClass("io/ionic/android_js_engine/Context"), "api", "Lio/ionic/android_js_engine/ContextAPI;");

  auto api = env->GetObjectField(thiz, context_api_field_id);
  auto api_instance = env->NewGlobalRef(api);

  auto *context = new Context(env->GetStringUTFChars(name, nullptr), runner->rt, env, api_instance);

  return (jlong)(long)context;
}
extern "C" JNIEXPORT void JNICALL Java_io_ionic_android_1js_1engine_Context_destroyContext(JNIEnv *env, jobject thiz, jlong ptr) {
  auto *ctx = (Context *)ptr;
  delete ctx;
}
extern "C" JNIEXPORT jstring JNICALL Java_io_ionic_android_1js_1engine_Context_evaluate(JNIEnv *env, jobject thiz, jlong ptr, jstring code, jboolean ret_value) {
  auto *ctx = (Context *)ptr;

  const char *c_code = env->GetStringUTFChars(code, nullptr);

  auto value = ctx->evaluate(c_code, (bool)ret_value);

  if (check_and_throw_js_exception(env, ctx->ctx, value)) {
    return nullptr;
  }

  const char *json_string = JS_ToCString(ctx->ctx, value);

  JS_FreeValue(ctx->ctx, value);

  return env->NewStringUTF(json_string);
}
extern "C" JNIEXPORT void JNICALL Java_io_ionic_android_1js_1engine_Context_start(JNIEnv *env, jobject thiz, jlong ptr) {
  auto *ctx = (Context *)ptr;
  ctx->start_run_loop();
}
extern "C" JNIEXPORT void JNICALL Java_io_ionic_android_1js_1engine_Context_stop(JNIEnv *env, jobject thiz, jlong ptr) {
  auto *ctx = (Context *)ptr;
  ctx->stop_run_loop();
}
extern "C" JNIEXPORT void JNICALL Java_io_ionic_android_1js_1engine_Context_dispatchEvent(JNIEnv *env, jobject thiz, jlong ptr, jstring event, jstring details) {
  auto *ctx = (Context *)ptr;

  const char *c_event = env->GetStringUTFChars(event, nullptr);
  const char *c_details_json = env->GetStringUTFChars(details, nullptr);

  JSValue details_obj = ctx->parseJSON(c_details_json);

  JSValue value = ctx->dispatch_event(c_event, details_obj);
  JS_FreeValue(ctx->ctx, details_obj);

  env->ReleaseStringUTFChars(event, c_event);
  env->ReleaseStringUTFChars(details, c_details_json);

  if (check_and_throw_js_exception(env, ctx->ctx, value)) {
    return;
  }

  JS_FreeValue(ctx->ctx, value);
}
extern "C" JNIEXPORT void JNICALL Java_io_ionic_android_1js_1engine_Context_registerGlobalFunction(JNIEnv *env, jobject thiz, jlong ptr, jstring function_name, jobject function) {
  auto c_function_name = env->GetStringUTFChars(function_name, nullptr);

  auto *ctx = (Context *)ptr;
  ctx->register_function(c_function_name, env->NewGlobalRef(function));

  env->ReleaseStringUTFChars(function_name, c_function_name);
}
extern "C" JNIEXPORT void JNICALL Java_io_ionic_android_1js_1engine_Context_initCapacitorAPI(JNIEnv *env, jobject thiz, jlong ptr, jobject api) {
  auto *ctx = (Context *)ptr;

  auto api_instance = env->NewGlobalRef(api);
  ctx->init_capacitor_api(api_instance);
}