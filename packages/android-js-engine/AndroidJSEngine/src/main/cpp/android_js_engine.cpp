#include <jni.h>
#include <string>
#include "./js-engine/src/native.hpp"

extern "C" JNIEXPORT jstring JNICALL
Java_io_ionic_android_1js_1engine_NativeLib_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";


    return env->NewStringUTF(hello.c_str());


}