#ifndef ANDROID_JS_ENGINE_ERRORS_H
#define ANDROID_JS_ENGINE_ERRORS_H

#include <jni.h>
#include <exception>
#include <string>
#include "js-engine/src/quickjs/quickjs.h"

JSValue get_jvm_exception(JNIEnv *env, JSContext *ctx);
JSValue get_js_exception(JSContext *ctx);
JSValue create_js_error(const char * error_message, JSContext *ctx);
bool throw_js_error_in_jvm(JNIEnv *env, JSContext *ctx, JSValue value);

class NativeInterfaceException : public std::exception {
public:
    NativeInterfaceException(const char* error);
    const char* what() const throw();

private:
    const char* js_exception;
    std::string message;
};


#endif //ANDROID_JS_ENGINE_ERRORS_H
