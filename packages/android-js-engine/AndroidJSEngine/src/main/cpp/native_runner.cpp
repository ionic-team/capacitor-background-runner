#include <jni.h>

#include "./js-engine/src/runner.hpp"
#include "native_android_interface.h"
extern "C" JNIEXPORT jlong JNICALL Java_io_ionic_android_1js_1engine_Runner_initRunner(JNIEnv *env, jobject thiz) {
  auto *native = new NativeAndroidInterface(env);
  auto *runner = new Runner(native);

  return (jlong)(long)runner;
}
extern "C" JNIEXPORT jboolean JNICALL Java_io_ionic_android_1js_1engine_Runner_hasPendingJobs(JNIEnv *env, jobject thiz, jlong ptr) {
  auto *runner = (Runner *)ptr;
  return runner->has_pending_jobs();
}

extern "C" JNIEXPORT void JNICALL Java_io_ionic_android_1js_1engine_Runner_executePendingJobs(JNIEnv *env, jobject thiz, jlong ptr) {
  auto *runner = (Runner *)ptr;
  runner->execute_pending_jobs();
}
extern "C" JNIEXPORT void JNICALL Java_io_ionic_android_1js_1engine_Runner_destroyRunner(JNIEnv *env, jobject thiz, jlong ptr) {
  auto *runner = (Runner *)ptr;
  delete runner;
}