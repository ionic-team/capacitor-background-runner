#include "jni_classes.h"

#include <stdexcept>

JNIClasses::JNIClasses(JNIEnv *env) {
  // get all needed JNI classes and methods

  jclass tempClass;

  // Context API
  tempClass = env->FindClass("io/ionic/android_js_engine/ContextAPI");
  this->check_exception(env);
  this->context_api_class = (jclass)env->NewGlobalRef(tempClass);
  env->DeleteLocalRef(tempClass);
  tempClass = nullptr;

  tempClass = env->FindClass("io/ionic/android_js_engine/JSResponse");
  this->check_exception(env);
  this->js_response_class = (jclass)env->NewGlobalRef(tempClass);
  env->DeleteLocalRef(tempClass);
  tempClass = nullptr;

  tempClass = env->FindClass("io/ionic/android_js_engine/JSFetchOptions");
  this->check_exception(env);
  this->js_fetch_options_class = (jclass)env->NewGlobalRef(tempClass);
  env->DeleteLocalRef(tempClass);
  tempClass = nullptr;

  this->context_api_fetch_method = env->GetMethodID(this->context_api_class, "fetch", "(Ljava/lang/String;Lio/ionic/android_js_engine/JSFetchOptions;)Lio/ionic/android_js_engine/JSResponse;");
  this->check_exception(env);

  this->context_api_byteArrayToString_method = env->GetMethodID(this->context_api_class, "byteArrayToString", "([BLjava/lang/String;)Ljava/lang/String;");
  this->check_exception(env);

  this->js_fetch_options_constructor = env->GetMethodID(this->js_fetch_options_class, "<init>", "(Ljava/lang/String;)V");
  this->check_exception(env);

  this->js_response_ok_field = env->GetFieldID(this->js_response_class, "ok", "Z");
  this->check_exception(env);

  this->js_response_status_field = env->GetFieldID(this->js_response_class, "status", "I");
  this->check_exception(env);

  this->js_response_url_field = env->GetFieldID(this->js_response_class, "url", "Ljava/lang/String;");
  this->check_exception(env);

  this->js_response_data_field = env->GetFieldID(this->js_response_class, "data", "[B");
  this->check_exception(env);

  this->js_response_error_field = env->GetFieldID(this->js_response_class, "error", "Ljava/lang/String;");
  this->check_exception(env);

  // Capacitor API
  tempClass = env->FindClass("io/ionic/android_js_engine/api/CapacitorAPI");
  this->check_exception(env);
  this->capacitor_api_class = (jclass)env->NewGlobalRef(tempClass);
  env->DeleteLocalRef(tempClass);
  tempClass = nullptr;

  this->capacitor_api_kv_field = env->GetFieldID(this->capacitor_api_class, "kv", "Lio/ionic/android_js_engine/api/KVAPI;");
  this->check_exception(env);

  this->capacitor_api_device_field = env->GetFieldID(this->capacitor_api_class, "device", "Lio/ionic/android_js_engine/api/DeviceAPI;");
  this->check_exception(env);

  this->capacitor_api_geolocation_field = env->GetFieldID(this->capacitor_api_class, "geolocation", "Lio/ionic/android_js_engine/api/GeolocationAPI;");
  this->check_exception(env);

  this->capacitor_api_notification_field = env->GetFieldID(this->capacitor_api_class, "notifications", "Lio/ionic/android_js_engine/api/NotificationsAPI;");
  this->check_exception(env);

  tempClass = env->FindClass("io/ionic/android_js_engine/api/KVAPI");
  this->check_exception(env);
  this->kv_api_class = (jclass)env->NewGlobalRef(tempClass);
  env->DeleteLocalRef(tempClass);
  tempClass = nullptr;

  tempClass = env->FindClass("io/ionic/android_js_engine/api/NotificationsAPI");
  this->check_exception(env);
  this->notification_api_class = (jclass)env->NewGlobalRef(tempClass);
  env->DeleteLocalRef(tempClass);
  tempClass = nullptr;

  tempClass = env->FindClass("io/ionic/android_js_engine/api/DeviceAPI");
  this->check_exception(env);
  this->device_api_class = (jclass)env->NewGlobalRef(tempClass);
  env->DeleteLocalRef(tempClass);
  tempClass = nullptr;

  tempClass = env->FindClass("io/ionic/android_js_engine/api/GeolocationAPI");
  this->check_exception(env);
  this->geolocation_api_class = (jclass)env->NewGlobalRef(tempClass);
  env->DeleteLocalRef(tempClass);
  tempClass = nullptr;
}

void JNIClasses::check_exception(JNIEnv *env) {
  if (!env->ExceptionCheck()) {
    return;
  }

  auto throwable = env->ExceptionOccurred();
  env->ExceptionClear();

  jclass c = env->FindClass("java/lang/Exception");
  jmethodID cnstrctr = env->GetMethodID(c, "<init>", "(Ljava/lang/Throwable;)V");
  jmethodID getMessage = env->GetMethodID(c, "getMessage", "()Ljava/lang/String;");

  auto errObj = env->NewObject(c, cnstrctr, throwable);
  auto errMsg = (jstring)env->CallObjectMethod(errObj, getMessage);

  auto c_errMsg = env->GetStringUTFChars(errMsg, nullptr);

  throw std::runtime_error(c_errMsg);
}
