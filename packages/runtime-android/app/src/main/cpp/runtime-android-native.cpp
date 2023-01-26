#include <jni.h>
#include <stdlib.h>
#include "quickjspp.hpp"


jint jni_int_test() {
    return 123;
}

jint test_quick_js() {
    qjs::Runtime runtime;
    qjs::Context context(runtime);

    try {
        context.eval("const value = 1 + 2;");
        auto value = context.eval("value");

        auto intValue = (int) value;

        return (jint) intValue;
    } catch (qjs::exception &ex) {
        auto exception = context.getException();
        return -1;
    }
}

extern "C"
JNIEXPORT jint JNICALL
Java_io_ionic_backgroundrunner_android_Runner_00024Companion_integerFromJNI(JNIEnv *env, jobject thiz) {
    return (jint) test_quick_js();
}
extern "C"
JNIEXPORT jlong JNICALL
Java_io_ionic_backgroundrunner_android_Runner_00024Companion_initRunner(JNIEnv *env, jobject thiz) {
    qjs::Runtime* runtime = new qjs::Runtime();
    return (jlong)(long)runtime;
}
extern "C"
JNIEXPORT void JNICALL
Java_io_ionic_backgroundrunner_android_Runner_00024Companion_destroyRunner(JNIEnv *env, jobject thiz, jlong ptr) {
    qjs::Runtime* runtime = (qjs::Runtime*)ptr;
    delete runtime;
}

extern "C"
JNIEXPORT jlong JNICALL
Java_io_ionic_backgroundrunner_android_RunnerContext_00024Companion_initContext(JNIEnv *env, jobject thiz, jlong runner_ptr) {
    qjs::Runtime* runtime = (qjs::Runtime*)runner_ptr;
    qjs::Context context(*runtime);

    return (jlong)(long)context.ctx;
}

extern "C"
JNIEXPORT void JNICALL
Java_io_ionic_backgroundrunner_android_RunnerContext_00024Companion_destroyContext(JNIEnv *env, jobject thiz, jlong ptr) {
    // TODO: implement destroyContext()
}

extern "C"
JNIEXPORT jint JNICALL
Java_io_ionic_backgroundrunner_android_RunnerContext_00024Companion_execute(JNIEnv *env, jobject thiz, jlong ptr, jstring code) {
    qjs::Runtime* runtime = (qjs::Runtime*)ptr;
    qjs::Context context(*runtime);

    context.eval("const test = 1 + 2;");
    auto value = context.eval("test");

    return (jint)value;
}