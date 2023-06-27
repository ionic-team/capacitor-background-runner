#include <jni.h>
#include "quickjs/quickjs.h"
#include "quickjs/cutils.h"
#include "Context.h"

jint throw_js_exception(JNIEnv *env, JSContext *ctx) {
  jclass c = (*env).FindClass("io/ionic/backgroundrunner/EngineErrors$JavaScriptException");

  JSValue exception = JS_GetException(ctx);

  JSValue err_message = JS_GetPropertyStr(ctx, exception, "message");
  const char *err_str = JS_ToCString(ctx, err_message);

  auto ret = (*env).ThrowNew(c, err_str);

  JS_FreeCString(ctx, err_str);

  JS_FreeValue(ctx, err_message);
  JS_FreeValue(ctx, exception);

  return ret;
}

extern "C" JNIEXPORT jlong JNICALL Java_io_ionic_backgroundrunner_Context_initContext(JNIEnv *env, jobject thiz, jlong runner_ptr, jstring name) {
  auto *rt = (JSRuntime *)runner_ptr;

  auto context_api_field_id = env->GetFieldID(env->FindClass("io/ionic/backgroundrunner/Context"), "api", "Lio/ionic/backgroundrunner/ContextAPI;");

  auto api = env->GetObjectField(thiz, context_api_field_id);
  auto api_instance = env->NewGlobalRef(api);

  auto *context = new Context(env->GetStringUTFChars(name, nullptr), rt, env, api_instance);

  return (jlong)(long)context;
}

extern "C" JNIEXPORT void JNICALL Java_io_ionic_backgroundrunner_Context_destroyContext(JNIEnv *env, jobject thiz, jlong ptr) {
  auto *ctx = (Context *)ptr;
  delete ctx;
}

extern "C" JNIEXPORT jstring JNICALL Java_io_ionic_backgroundrunner_Context_evaluate(JNIEnv *env, jobject thiz, jlong ptr, jstring code, jboolean ret_value) {
  auto *ctx = (Context *)ptr;

  const char *c_code = env->GetStringUTFChars(code, nullptr);

  auto value = ctx->evaluate(c_code, (bool)ret_value);

  if (JS_IsException(value)) {
    JS_FreeValue(ctx->ctx, value);
    throw_js_exception(env, ctx->ctx);
    return nullptr;
  } else {
    const char * json_string = JS_ToCString(ctx->ctx, value);

    JS_FreeValue(ctx->ctx, value);

    return env->NewStringUTF(json_string);
  }
}

extern "C" JNIEXPORT void JNICALL Java_io_ionic_backgroundrunner_Context_start(JNIEnv *env, jobject thiz, jlong ptr) {
  auto *ctx = (Context *)ptr;
  ctx->start_run_loop();
}

extern "C" JNIEXPORT void JNICALL Java_io_ionic_backgroundrunner_Context_stop(JNIEnv *env, jobject thiz, jlong ptr) {
  auto *ctx = (Context *)ptr;
  ctx->stop_run_loop();
}

extern "C" JNIEXPORT void JNICALL Java_io_ionic_backgroundrunner_Context_dispatchEvent(JNIEnv *env, jobject thiz, jlong ptr, jstring event, jstring details) {
  auto *ctx = (Context *)ptr;

  const char *c_event = env->GetStringUTFChars(event, nullptr);
  const char *c_details_json = env->GetStringUTFChars(details, nullptr);

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

extern "C" JNIEXPORT void JNICALL Java_io_ionic_backgroundrunner_Context_registerGlobalFunction(JNIEnv *env, jobject thiz, jlong ptr, jstring function_name, jobject function) {
  auto c_function_name = env->GetStringUTFChars(function_name, nullptr);

  auto *ctx = (Context *)ptr;
  ctx->register_function(c_function_name, env->NewGlobalRef(function));

  env->ReleaseStringUTFChars(function_name, c_function_name);
}