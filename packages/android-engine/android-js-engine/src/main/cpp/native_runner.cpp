#include <jni.h>

#include "runner.h"

extern "C" JNIEXPORT jlong JNICALL Java_io_ionic_android_1js_1engine_Runner_initRunner(JNIEnv *env, jobject thiz) {
  auto *runner = new Runner();
  runner->start_run_loop();
  return (jlong)(long)runner;
}
extern "C" JNIEXPORT void JNICALL Java_io_ionic_android_1js_1engine_Runner_destroyRunner(JNIEnv *env, jobject thiz, jlong ptr) {
  auto *runner = (Runner *)ptr;
  delete runner;
}
extern "C"
JNIEXPORT void JNICALL
Java_io_ionic_android_1js_1engine_Runner_stopRunLoop(JNIEnv *env, jobject thiz, jlong ptr) {
  auto *runner = (Runner *)ptr;
  runner->stop_run_loop();
}