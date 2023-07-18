#ifndef CAPACITOR_BACKGROUND_RUNNER_ERRORS_H
#define CAPACITOR_BACKGROUND_RUNNER_ERRORS_H

#include <jni.h>
#include "quickjs/quickjs.h"

bool check_and_throw_js_exception(JNIEnv *env, JSContext *ctx, JSValue value);
JSValue check_and_throw_jni_exception(JNIEnv *env, JSContext *ctx);

#endif //CAPACITOR_BACKGROUND_RUNNER_ERRORS_H
