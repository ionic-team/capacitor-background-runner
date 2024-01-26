#include <jni.h>

#include "./js-engine/src/runner.hpp"
#include "native_android_interface.h"
extern "C" JNIEXPORT jlong JNICALL Java_io_ionic_android_1js_1engine_Runner_initRunner(JNIEnv *env, jobject thiz) {
  auto *native = new NativeAndroidInterface(env);
  auto *runner = new Runner(native);

  return (jlong)(long)runner;
}
extern "C" JNIEXPORT void JNICALL Java_io_ionic_android_1js_1engine_Runner_startRunner(JNIEnv *env, jobject thiz, jlong ptr) {
  auto *runner = (Runner *)ptr;
  runner->start();
}
extern "C" JNIEXPORT void JNICALL Java_io_ionic_android_1js_1engine_Runner_stopRunner(JNIEnv *env, jobject thiz, jlong ptr) {
  auto *runner = (Runner *)ptr;
  runner->stop();
}
extern "C" JNIEXPORT void JNICALL Java_io_ionic_android_1js_1engine_Runner_destroyRunner(JNIEnv *env, jobject thiz, jlong ptr) {
  auto *runner = (Runner *)ptr;
  delete runner;
}