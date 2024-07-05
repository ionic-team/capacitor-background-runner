#include "native_capacitor_interface.h"

#include "java_errors.h"
#include "js-engine/src/capacitor-api/api_device.h"

NativeCapacitorInterface::NativeCapacitorInterface(JNIEnv *env, jobject api) {
  this->java = new Java(env);
  this->api = api;
}

NativeCapacitorInterface::~NativeCapacitorInterface() {
  auto *env = this->java->getEnv();
  if (env == nullptr) {
    return;
  }

  env->DeleteGlobalRef(this->api);
  this->api = nullptr;
}

// Device API
std::string NativeCapacitorInterface::device_api_getBatteryStatus() {
  auto *env = this->java->getEnv();
  if (env == nullptr) {
    throw new NativeInterfaceException("JVM environment is null");
  }

  auto *device = env->GetObjectField(this->api, this->java->capacitor_api_device_field);
  auto jvm_exception = get_jvm_exception(env);
  if (jvm_exception != nullptr) {
    throw *jvm_exception;
  }

  auto *json_str_j = (jstring)env->CallObjectMethod(device, this->java->capacitor_api_device_getBatteryStatus_method);
  jvm_exception = get_jvm_exception(env);
  if (jvm_exception != nullptr) {
    throw *jvm_exception;
  }

  const auto *json_str_c = env->GetStringUTFChars(json_str_j, nullptr);
  std::string json_str(json_str_c);

  env->ReleaseStringUTFChars(json_str_j, json_str_c);

  return json_str;
}

std::string NativeCapacitorInterface::device_api_getNetworkStatus() {
  auto *env = this->java->getEnv();
  if (env == nullptr) {
    throw new NativeInterfaceException("JVM environment is null");
  }

  auto *device = env->GetObjectField(this->api, this->java->capacitor_api_device_field);
  auto jvm_exception = get_jvm_exception(env);
  if (jvm_exception != nullptr) {
    throw *jvm_exception;
  }

  auto *json_str_j = (jstring)env->CallObjectMethod(device, this->java->capacitor_api_device_getNetworkStatus_method);
  jvm_exception = get_jvm_exception(env);
  if (jvm_exception != nullptr) {
    throw *jvm_exception;
  }

  const auto *json_str_c = env->GetStringUTFChars(json_str_j, nullptr);
  std::string json_str(json_str_c);

  env->ReleaseStringUTFChars(json_str_j, json_str_c);

  return json_str;
}

// KV API
void NativeCapacitorInterface::kv_api_set(std::string key, std::string json_value) {
  auto *env = this->java->getEnv();
  if (env == nullptr) {
    throw new NativeInterfaceException("JVM environment is null");
  }

  auto *kv = env->GetObjectField(this->api, this->java->capacitor_api_kv_field);
  auto jvm_exception = get_jvm_exception(env);
  if (jvm_exception != nullptr) {
    throw *jvm_exception;
  }

  jstring key_j_str = env->NewStringUTF(key.c_str());
  jstring value_j_str = env->NewStringUTF(json_value.c_str());

  env->CallVoidMethod(kv, this->java->capacitor_api_kv_set_method, key_j_str, value_j_str);
  jvm_exception = get_jvm_exception(env);
  if (jvm_exception != nullptr) {
    throw *jvm_exception;
  }
}

std::string NativeCapacitorInterface::kv_api_get(std::string key) {
  auto *env = this->java->getEnv();
  if (env == nullptr) {
    throw new NativeInterfaceException("JVM environment is null");
  }

  auto *kv = env->GetObjectField(this->api, this->java->capacitor_api_kv_field);
  auto jvm_exception = get_jvm_exception(env);
  if (jvm_exception != nullptr) {
    throw *jvm_exception;
  }

  jstring key_j_str = env->NewStringUTF(key.c_str());

  auto *value_j_str = (jstring)env->CallObjectMethod(kv, this->java->capacitor_api_kv_get_method, key_j_str);
  jvm_exception = get_jvm_exception(env);
  if (jvm_exception != nullptr) {
    throw *jvm_exception;
  }

  if (value_j_str == nullptr) {
    return "";
  }

  const auto *value_c_str = env->GetStringUTFChars(value_j_str, nullptr);
  auto value = std::string(value_c_str);
  env->ReleaseStringUTFChars(value_j_str, value_c_str);

  return value;
}

void NativeCapacitorInterface::kv_api_remove(std::string key) {
  auto *env = this->java->getEnv();
  if (env == nullptr) {
    throw new NativeInterfaceException("JVM environment is null");
  }

  auto *kv = env->GetObjectField(this->api, this->java->capacitor_api_kv_field);
  auto jvm_exception = get_jvm_exception(env);
  if (jvm_exception != nullptr) {
    throw *jvm_exception;
  }

  jstring key_j_str = env->NewStringUTF(key.c_str());

  env->CallVoidMethod(kv, this->java->capacitor_api_kv_remove_method, key_j_str);
  jvm_exception = get_jvm_exception(env);
  if (jvm_exception != nullptr) {
    throw *jvm_exception;
  }
}

// Notifications API
void NativeCapacitorInterface::notifications_api_schedule(std::string options_json) {
  auto *env = this->java->getEnv();
  if (env == nullptr) {
    throw new NativeInterfaceException("JVM environment is null");
  }

  auto *notification = env->GetObjectField(this->api, this->java->capacitor_api_notification_field);
  auto jvm_exception = get_jvm_exception(env);
  if (jvm_exception != nullptr) {
    throw *jvm_exception;
  }

  auto *options_j_str = env->NewStringUTF(options_json.c_str());

  env->CallVoidMethod(notification, this->java->capacitor_api_notifications_schedule_method, options_j_str);
  jvm_exception = get_jvm_exception(env);
  if (jvm_exception != nullptr) {
    throw *jvm_exception;
  }
}

void NativeCapacitorInterface::notifications_api_setBadge(int value) {
  auto *env = this->java->getEnv();
  if (env == nullptr) {
    throw new NativeInterfaceException("JVM environment is null");
  }

  auto *notification = env->GetObjectField(this->api, this->java->capacitor_api_notification_field);
  auto jvm_exception = get_jvm_exception(env);
  if (jvm_exception != nullptr) {
    throw *jvm_exception;
  }

  env->CallVoidMethod(notification, this->java->capacitor_api_notifications_setBadge_method, value);
  jvm_exception = get_jvm_exception(env);
  if (jvm_exception != nullptr) {
    throw *jvm_exception;
  }
}

void NativeCapacitorInterface::notifications_api_clearBadge() {
  auto *env = this->java->getEnv();
  if (env == nullptr) {
    throw new NativeInterfaceException("JVM environment is null");
  }

  auto *notification = env->GetObjectField(this->api, this->java->capacitor_api_notification_field);
  auto jvm_exception = get_jvm_exception(env);
  if (jvm_exception != nullptr) {
    throw *jvm_exception;
  }

  env->CallVoidMethod(notification, this->java->capacitor_api_notifications_clearBadge_method);
  jvm_exception = get_jvm_exception(env);
  if (jvm_exception != nullptr) {
    throw *jvm_exception;
  }
}

// Geolocation API
std::string NativeCapacitorInterface::geolocation_api_getCurrentPosition() {
  auto *env = this->java->getEnv();
  if (env == nullptr) {
    throw new NativeInterfaceException("JVM environment is null");
  }

  auto *geolocation = env->GetObjectField(this->api, this->java->capacitor_api_geolocation_field);
  auto jvm_exception = get_jvm_exception(env);
  if (jvm_exception != nullptr) {
    throw *jvm_exception;
  }

  auto value_j_str = (jstring)env->CallObjectMethod(geolocation, this->java->capacitor_api_geolocation_getCurrentPosition_method);
  jvm_exception = get_jvm_exception(env);
  if (jvm_exception != nullptr) {
    throw *jvm_exception;
  }

  if (value_j_str == nullptr) {
    return "";
  }

  const auto *c_json_str = env->GetStringUTFChars(value_j_str, nullptr);

  auto json = std::string(c_json_str);
  env->ReleaseStringUTFChars(value_j_str, c_json_str);

  return json;
}

// App API
std::string NativeCapacitorInterface::app_api_getInfo() {
  auto *env = this->java->getEnv();
  if (env == nullptr) {
    throw new NativeInterfaceException("JVM environment is null");
  }

  auto *app = env->GetObjectField(this->api, this->java->capacitor_api_app_field);
  auto jvm_exception = get_jvm_exception(env);
  if (jvm_exception != nullptr) {
    throw *jvm_exception;
  }

  auto value_j_str = (jstring)env->CallObjectMethod(app, this->java->capacitor_api_app_getInfo_method);
  jvm_exception = get_jvm_exception(env);
  if (jvm_exception != nullptr) {
    throw *jvm_exception;
  }

  if (value_j_str == nullptr) {
    return "";
  }

  const auto *c_json_str = env->GetStringUTFChars(value_j_str, nullptr);

  auto json = std::string(c_json_str);
  env->ReleaseStringUTFChars(value_j_str, c_json_str);

  return json;
}

std::string NativeCapacitorInterface::app_api_getState() {
  auto *env = this->java->getEnv();
  if (env == nullptr) {
    throw new NativeInterfaceException("JVM environment is null");
  }

  auto *app = env->GetObjectField(this->api, this->java->capacitor_api_app_field);
  auto jvm_exception = get_jvm_exception(env);
  if (jvm_exception != nullptr) {
    throw *jvm_exception;
  }

  auto value_j_str = (jstring)env->CallObjectMethod(app, this->java->capacitor_api_app_getState_method);
  jvm_exception = get_jvm_exception(env);
  if (jvm_exception != nullptr) {
    throw *jvm_exception;
  }

  if (value_j_str == nullptr) {
    return "";
  }

  const auto *c_json_str = env->GetStringUTFChars(value_j_str, nullptr);

  auto json = std::string(c_json_str);
  env->ReleaseStringUTFChars(value_j_str, c_json_str);

  return json;
}
