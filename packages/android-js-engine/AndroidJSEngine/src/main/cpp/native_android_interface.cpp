#include <android/log.h>

#include "errors.h"
#include "native_android_interface.h"

void write_to_logcat(android_LogPriority priority, const char *tag, const char *message) { __android_log_write(priority, tag, message); }

NativeAndroidInterface::NativeAndroidInterface(JNIEnv *env) {
    this->java = new Java(env);
}

NativeAndroidInterface::~NativeAndroidInterface() {
    delete this->java;
}

void NativeAndroidInterface::logger(LoggerLevel level, const std::string& tag, const std::string& messages) {
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

void NativeAndroidInterface::register_native_function(const std::string& func_name, std::any func_obj) {
    auto unwrapped_func_obj = std::any_cast<jobject>(func_obj);
    this->registered_functions.insert_or_assign(func_name, unwrapped_func_obj);
}

bool NativeAndroidInterface::has_native_function(const std::string& func_name) {
    return this->registered_functions.find(func_name) != this->registered_functions.end();
}

JSValue NativeAndroidInterface::invoke_native_function(const std::string& func_name, JSContext* ctx, JSValue args) {
    auto java_function_obj = this->registered_functions.at(func_name);

    auto *env = this->java->getEnv();
    if (env == nullptr) {
        return JS_Throw(ctx, create_js_error("JVM environment is null", ctx));
    }

    auto java_function_class = env->GetObjectClass(java_function_obj);
    auto jvm_exception = get_jvm_exception(env, ctx);
    if (!JS_IsNull(jvm_exception) && JS_IsError(ctx, jvm_exception)) {
        return JS_Throw(ctx, jvm_exception);
    }

    auto java_method = env->GetMethodID(java_function_class, "run", "()V");
    jvm_exception = get_jvm_exception(env, ctx);
    if (!JS_IsNull(jvm_exception) && JS_IsError(ctx, jvm_exception)) {
        return JS_Throw(ctx, jvm_exception);
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
        jvm_exception = get_jvm_exception(env, ctx);
        if (!JS_IsNull(jvm_exception) && JS_IsError(ctx, jvm_exception)) {
            return JS_Throw(ctx, jvm_exception);
        }

        jobject json_object = env->NewObject(json_object_class, json_object_constructor, json_j_string);
        jvm_exception = get_jvm_exception(env, ctx);
        if (!JS_IsNull(jvm_exception) && JS_IsError(ctx, jvm_exception)) {
            return JS_Throw(ctx, jvm_exception);
        }

        jfieldID args_field = env->GetFieldID(java_function_class, "jsFunctionArgs", "Lorg/json/JSONObject;");
        env->SetObjectField(java_function_obj, args_field, json_object);
        jvm_exception = get_jvm_exception(env, ctx);
        if (!JS_IsNull(jvm_exception) && JS_IsError(ctx, jvm_exception)) {
            return JS_Throw(ctx, jvm_exception);
        }
    }

    env->CallVoidMethod(java_function_obj, java_method);
    jvm_exception = get_jvm_exception(env, ctx);
    if (!JS_IsNull(jvm_exception) && JS_IsError(ctx, jvm_exception)) {
        return JS_Throw(ctx, jvm_exception);
    }

    return JS_UNDEFINED;
}

std::string NativeAndroidInterface::crypto_get_random_uuid() {
    auto *env = this->java->getEnv();
    if (env == nullptr) {
        return "";
    }

    auto *str = (jstring) env->CallStaticObjectMethod(this->java->web_api_class, this->java->web_api_cryptoRandomUUID_method);
    const auto *c_str = env->GetStringUTFChars(str, nullptr);

    std::string return_string(c_str);
    env->ReleaseStringUTFChars(str, c_str);

    return return_string;
}

std::vector<uint8_t> NativeAndroidInterface::crypto_get_random(size_t size) {
    auto *env = this->java->getEnv();
    if (env == nullptr) {
        // TODO:
        return std::vector<uint8_t>();
    }

    auto random_vector = std::vector<uint8_t>(size);

    auto random_bytes = static_cast<jbyteArray>(env->CallStaticObjectMethod(this->java->web_api_class, this->java->web_api_cryptoGetRandom_method, size));
    auto random = env->GetByteArrayElements(random_bytes, nullptr);

    for (int i = 0; i < size; i++) {
        random_vector[i] = random[i];
    }

    env->ReleaseByteArrayElements(random_bytes, random, 0);

    return random_vector;
}

int NativeAndroidInterface::get_random_hash() {
    auto *env = this->java->getEnv();
    if (env == nullptr) {
        // TODO:
        return 0;
    }

    const int unique = env->CallStaticIntMethod(this->java->web_api_class, this->java->web_api_randomHashCode_method);
    // TODO: check for errors

    return unique;
}

NativeResponse NativeAndroidInterface::fetch(NativeRequest request) {
    NativeResponse native_response;

    return native_response;
}

std::string NativeAndroidInterface::byte_array_to_str(uint8_t* arr, size_t size, const std::string& encoding) {
    auto *env = this->java->getEnv();
    if (env == nullptr) {
        // TODO: throw cpp exceptions
        return 0;
    }

    auto *j_encoding = env->NewStringUTF(encoding.c_str());
    jbyteArray byte_array = env->NewByteArray(size);

    env->SetByteArrayRegion(byte_array, 0, size, reinterpret_cast<const jbyte *>(arr));

    auto *str_j = (jstring)env->CallStaticObjectMethod(this->java->web_api_class, this->java->web_api_byteArrayToString_method, byte_array, j_encoding);
    const char *c_str = env->GetStringUTFChars(str_j, nullptr);

    auto str = std::string(c_str);

    env->ReleaseStringUTFChars(str_j, c_str);

    return str;
}

std::vector<uint8_t> NativeAndroidInterface::string_to_byte_array(std::string str) {
    auto *env = this->java->getEnv();
    if (env == nullptr) {
        // TODO:
        return std::vector<uint8_t>();
    }

    jstring j_string = env->NewStringUTF(str.c_str());
    auto *byte_array = static_cast<jbyteArray>(env->CallStaticObjectMethod(this->java->web_api_class, this->java->web_api_stringToByteArray_method, j_string));
    auto length = env->GetArrayLength(byte_array);
    auto arr = env->GetByteArrayElements(byte_array, 0);

    auto vector_bytes = std::vector<uint8_t>(length);
    for (int i = 0; i < length; i++) {
        vector_bytes[i] = arr[i];
    }

    env->ReleaseByteArrayElements(byte_array, arr, 0);

    return vector_bytes;
}
