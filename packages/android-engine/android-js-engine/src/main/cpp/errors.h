#ifndef CAPACITOR_BACKGROUND_RUNNER_ERRORS_H
#define CAPACITOR_BACKGROUND_RUNNER_ERRORS_H

#include <jni.h>
#include "quickjs/quickjs.h"

bool throw_js_exception_in_jvm(JNIEnv *env, JSContext *ctx, JSValue value);
JSValue throw_jvm_exception_in_js(JNIEnv *env, JSContext *ctx);
JSValue throw_js_exception(JSContext *ctx, const char * message);
void reject_promise(JSContext *ctx, JSValue reject_func, JSValue reject_obj);

#endif //CAPACITOR_BACKGROUND_RUNNER_ERRORS_H
