#include <jni.h>
#include "quickjs/quickjs.h"

extern "C"
JNIEXPORT jlong JNICALL
Java_io_ionic_backgroundrunner_Runner_initRunner(JNIEnv *env, jobject thiz) {
    JSRuntime *rt = JS_NewRuntime();
    JS_SetCanBlock(rt, 1);
    JS_SetMaxStackSize(rt, 0);

    return (jlong)(long)rt;

}
extern "C"
JNIEXPORT void JNICALL
Java_io_ionic_backgroundrunner_Runner_destroyRunner(JNIEnv *env, jobject thiz, jlong ptr) {
    auto *rt = (JSRuntime *)ptr;
    JS_FreeRuntime(rt);
}