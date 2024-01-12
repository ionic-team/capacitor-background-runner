#ifndef ANDROID_JS_ENGINE_JAVA_ERRORS_H
#define ANDROID_JS_ENGINE_JAVA_ERRORS_H

#include <jni.h>
#include "js-engine/src/quickjs/quickjs.h"
#include "js-engine/src/errors.hpp"

NativeInterfaceException* get_jvm_exception(JNIEnv *env);
bool throw_js_error_in_jvm(JNIEnv *env, JSContext *ctx, JSValue value);

#endif //ANDROID_JS_ENGINE_JAVA_ERRORS_H
