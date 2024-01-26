#include "native_android_interface.h"

#include <android/log.h>

#include "java_errors.h"

void write_to_logcat(android_LogPriority priority, const char *tag, const char *message) { __android_log_write(priority, tag, message); }

NativeAndroidInterface::NativeAndroidInterface(JNIEnv *env) { this->java = new Java(env); }

NativeAndroidInterface::~NativeAndroidInterface() { delete this->java; }

void NativeAndroidInterface::logger(LoggerLevel level, const std::string &tag, const std::string &messages) {
  android_LogPriority priority = ANDROID_LOG_DEFAULT;

  switch (level) {
    case LoggerLevel::INFO:
      priority = ANDROID_LOG_INFO;
      break;
    case LoggerLevel::DEBUG:
      priority = ANDROID_LOG_DEBUG;
      break;
    case LoggerLevel::ERROR:
      priority = ANDROID_LOG_ERROR;
      break;
    case LoggerLevel::WARN:
      priority = ANDROID_LOG_WARN;
      break;
  }

  write_to_logcat(priority, tag.c_str(), messages.c_str());
}

void NativeAndroidInterface::register_native_function(const std::string &func_name, std::any func_obj) {
  auto unwrapped_func_obj = std::any_cast<jobject>(func_obj);
  this->registered_functions.insert_or_assign(func_name, unwrapped_func_obj);
}

bool NativeAndroidInterface::has_native_function(const std::string &func_name) { return this->registered_functions.find(func_name) != this->registered_functions.end(); }

JSValue NativeAndroidInterface::invoke_native_function(const std::string &func_name, JSContext *ctx, JSValue args) {
  auto java_function_obj = this->registered_functions.at(func_name);

  auto *env = this->java->getEnv();
  if (env == nullptr) {
    throw new NativeInterfaceException("JVM environment is null");
  }

  auto java_function_class = env->GetObjectClass(java_function_obj);
  auto jvm_exception = get_jvm_exception(env);
  if (jvm_exception != nullptr) {
    throw *jvm_exception;
  }

  auto java_method = env->GetMethodID(java_function_class, "run", "()V");
  jvm_exception = get_jvm_exception(env);
  if (jvm_exception != nullptr) {
    throw *jvm_exception;
  }

  if (!JS_IsNull(args) && !JS_IsUndefined(args)) {
    std::string json_string;

    if (JS_IsError(ctx, args)) {
      auto error_object = JS_NewObject(ctx);

      auto error_name = JS_GetPropertyStr(ctx, args, "name");
      auto error_message = JS_GetPropertyStr(ctx, args, "message");

      JS_SetPropertyStr(ctx, error_object, "name", error_name);
      JS_SetPropertyStr(ctx, error_object, "message", error_message);

      auto json_string_obj = JS_JSONStringify(ctx, error_object, JS_UNDEFINED, JS_UNDEFINED);
      auto json_c_string = JS_ToCString(ctx, json_string_obj);

      json_string = std::string(json_c_string);

      JS_FreeCString(ctx, json_c_string);
      JS_FreeValue(ctx, json_string_obj);
      JS_FreeValue(ctx, error_object);
    } else {
      auto json_string_obj = JS_JSONStringify(ctx, args, JS_UNDEFINED, JS_UNDEFINED);
      auto json_c_str = JS_ToCString(ctx, json_string_obj);

      json_string = std::string(json_c_str);

      JS_FreeCString(ctx, json_c_str);
      JS_FreeValue(ctx, json_string_obj);
    }

    jstring json_j_string = env->NewStringUTF(json_string.c_str());

    // create a JSONObject
    jclass json_object_class = env->FindClass("org/json/JSONObject");
    jmethodID json_object_constructor = env->GetMethodID(json_object_class, "<init>", "(Ljava/lang/String;)V");
    jvm_exception = get_jvm_exception(env);
    if (jvm_exception != nullptr) {
      throw *jvm_exception;
    }

    jobject json_object = env->NewObject(json_object_class, json_object_constructor, json_j_string);
    jvm_exception = get_jvm_exception(env);
    if (jvm_exception != nullptr) {
      throw *jvm_exception;
    }

    jfieldID args_field = env->GetFieldID(java_function_class, "jsFunctionArgs", "Lorg/json/JSONObject;");
    env->SetObjectField(java_function_obj, args_field, json_object);
    jvm_exception = get_jvm_exception(env);
    if (jvm_exception != nullptr) {
      throw *jvm_exception;
    }
  }

  env->CallVoidMethod(java_function_obj, java_method);
  jvm_exception = get_jvm_exception(env);
  if (jvm_exception != nullptr) {
    throw *jvm_exception;
  }

  return JS_UNDEFINED;
}

std::string NativeAndroidInterface::crypto_get_random_uuid() {
  auto *env = this->java->getEnv();
  if (env == nullptr) {
    throw new NativeInterfaceException("JVM environment is null");
  }

  auto *str = (jstring)env->CallStaticObjectMethod(this->java->web_api_class, this->java->web_api_cryptoRandomUUID_method);
  auto jvm_exception = get_jvm_exception(env);
  if (jvm_exception != nullptr) {
    throw *jvm_exception;
  }

  const auto *c_str = env->GetStringUTFChars(str, nullptr);

  std::string return_string(c_str);
  env->ReleaseStringUTFChars(str, c_str);

  return return_string;
}

std::vector<uint8_t> NativeAndroidInterface::crypto_get_random(size_t size) {
  auto *env = this->java->getEnv();
  if (env == nullptr) {
    throw new NativeInterfaceException("JVM environment is null");
  }

  auto random_vector = std::vector<uint8_t>(size);

  auto random_bytes = static_cast<jbyteArray>(env->CallStaticObjectMethod(this->java->web_api_class, this->java->web_api_cryptoGetRandom_method, size));
  auto jvm_exception = get_jvm_exception(env);
  if (jvm_exception != nullptr) {
    throw *jvm_exception;
  }

  auto random = env->GetByteArrayElements(random_bytes, nullptr);
  jvm_exception = get_jvm_exception(env);
  if (jvm_exception != nullptr) {
    throw *jvm_exception;
  }

  for (int i = 0; i < size; i++) {
    random_vector[i] = random[i];
  }

  env->ReleaseByteArrayElements(random_bytes, random, 0);

  return random_vector;
}

int NativeAndroidInterface::get_random_hash() {
  auto *env = this->java->getEnv();
  if (env == nullptr) {
    throw new NativeInterfaceException("JVM environment is null");
  }

  const int unique = env->CallStaticIntMethod(this->java->web_api_class, this->java->web_api_randomHashCode_method);
  auto jvm_exception = get_jvm_exception(env);
  if (jvm_exception != nullptr) {
    throw *jvm_exception;
  }

  return unique;
}

jobject NativeAndroidInterface::native_request_to_native_js_fetch_options(JNIEnv *env, NativeRequest request) {
  auto http_method_j = env->NewStringUTF(request.method.c_str());
  jbyteArray byte_array = nullptr;

  if (!request.body.empty()) {
    byte_array = env->NewByteArray(request.body.size());
    env->SetByteArrayRegion(byte_array, 0, request.body.size(), reinterpret_cast<const jbyte *>(request.body.data()));
  }

  // creating HashMap
  jclass hash_map_class = env->FindClass("java/util/HashMap");
  jmethodID hash_map_constructor = env->GetMethodID(hash_map_class, "<init>", "(I)V");
  jobject headers_j = env->NewObject(hash_map_class, hash_map_constructor, (jsize)request.headers.size());
  auto jvm_exception = get_jvm_exception(env);
  if (jvm_exception != nullptr) {
    throw *jvm_exception;
  }

  jmethodID hash_map_put = env->GetMethodID(hash_map_class, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");

  for (const auto &kv : request.headers) {
    auto key_j = env->NewStringUTF(kv.first.c_str());
    auto value_j = env->NewStringUTF(kv.second.c_str());

    env->CallObjectMethod(headers_j, hash_map_put, key_j, value_j);
    jvm_exception = get_jvm_exception(env);
    if (jvm_exception != nullptr) {
      throw *jvm_exception;
    }
  }

  env->DeleteLocalRef(hash_map_class);

  // create native js fetch option
  jobject fetch_options_j = env->NewObject(this->java->native_js_fetch_options_class, this->java->native_js_fetch_options_constructor, http_method_j, headers_j, byte_array);
  jvm_exception = get_jvm_exception(env);
  if (jvm_exception != nullptr) {
    throw *jvm_exception;
  }

  auto return_fetch_options_j = env->NewGlobalRef(fetch_options_j);

  env->DeleteLocalRef(fetch_options_j);

  return return_fetch_options_j;
}

NativeResponse NativeAndroidInterface::native_js_response_to_native_response(JNIEnv *env, jobject native_js_response) {
  NativeResponse response;

  auto j_ok = env->GetBooleanField(native_js_response, this->java->native_js_response_ok_field);
  auto j_status = env->GetIntField(native_js_response, this->java->native_js_response_status_field);
  auto *j_url = (jstring)env->GetObjectField(native_js_response, this->java->native_js_response_url_field);
  auto *j_data = static_cast<jbyteArray>(env->GetObjectField(native_js_response, this->java->native_js_response_data_field));
  auto *j_err = (jstring)env->GetObjectField(native_js_response, this->java->native_js_response_error_field);

  response.status = (int)j_status;
  response.ok = (bool)j_ok;
  response.url = env->GetStringUTFChars(j_url, 0);

  if (j_data != nullptr) {
    auto length = env->GetArrayLength(j_data);
    auto data_arr = env->GetByteArrayElements(j_data, 0);

    std::vector<uint8_t> buffer(length);

    for (int i = 0; i < length; i++) {
      buffer[i] = data_arr[i];
    }

    env->ReleaseByteArrayElements(j_data, data_arr, 0);

    response.data = buffer;
  }

  if (j_err != nullptr) {
    response.error = env->GetStringUTFChars(j_err, 0);
  }

  return response;
}

NativeResponse NativeAndroidInterface::fetch(NativeRequest request) {
  auto *env = this->java->getEnv();
  if (env == nullptr) {
    throw new NativeInterfaceException("JVM environment is null");
  }

  NativeResponse native_response;

  jstring url = env->NewStringUTF(request.url.c_str());
  jobject fetch_options = this->native_request_to_native_js_fetch_options(env, request);

  jobject response = env->CallStaticObjectMethod(this->java->web_api_class, this->java->web_api_fetch_method, url, fetch_options);
  auto jvm_exception = get_jvm_exception(env);
  if (jvm_exception != nullptr) {
    throw *jvm_exception;
  }

  native_response = this->native_js_response_to_native_response(env, response);

  return native_response;
}

std::string NativeAndroidInterface::byte_array_to_str(uint8_t *arr, size_t size, const std::string &encoding) {
  auto *env = this->java->getEnv();
  if (env == nullptr) {
    throw new NativeInterfaceException("JVM environment is null");
  }

  auto *j_encoding = env->NewStringUTF(encoding.c_str());
  jbyteArray byte_array = env->NewByteArray(size);

  env->SetByteArrayRegion(byte_array, 0, size, reinterpret_cast<const jbyte *>(arr));

  auto *str_j = (jstring)env->CallStaticObjectMethod(this->java->web_api_class, this->java->web_api_byteArrayToString_method, byte_array, j_encoding);
  auto jvm_exception = get_jvm_exception(env);
  if (jvm_exception != nullptr) {
    throw *jvm_exception;
  }

  const char *c_str = env->GetStringUTFChars(str_j, nullptr);

  auto str = std::string(c_str);

  env->ReleaseStringUTFChars(str_j, c_str);

  return str;
}

std::vector<uint8_t> NativeAndroidInterface::string_to_byte_array(std::string str) {
  auto *env = this->java->getEnv();
  if (env == nullptr) {
    throw new NativeInterfaceException("JVM environment is null");
  }

  jstring j_string = env->NewStringUTF(str.c_str());
  auto *byte_array = static_cast<jbyteArray>(env->CallStaticObjectMethod(this->java->web_api_class, this->java->web_api_stringToByteArray_method, j_string));
  auto jvm_exception = get_jvm_exception(env);
  if (jvm_exception != nullptr) {
    throw *jvm_exception;
  }

  auto length = env->GetArrayLength(byte_array);
  auto arr = env->GetByteArrayElements(byte_array, 0);

  auto vector_bytes = std::vector<uint8_t>(length);
  for (int i = 0; i < length; i++) {
    vector_bytes[i] = arr[i];
  }

  env->ReleaseByteArrayElements(byte_array, arr, 0);

  return vector_bytes;
}
