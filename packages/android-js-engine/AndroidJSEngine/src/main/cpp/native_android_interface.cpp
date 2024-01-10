#include <android/log.h>

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
//        throw_js_exception(ctx, "JVM Environment is null");
        return JS_UNDEFINED;
    }

    auto java_function_class = env->GetObjectClass(java_function_obj);
//    auto exception = throw_jvm_exception_in_js(env, ctx);
//    if (JS_IsException(exception)) {
//        return exception;
//    }

    auto java_method = env->GetMethodID(java_function_class, "run", "()V");
//    exception = throw_jvm_exception_in_js(env, ctx);
//    if (JS_IsException(exception)) {
//        return exception;
//    }

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
//        exception = throw_jvm_exception_in_js(env, ctx);
//        if (JS_IsException(exception)) {
//            return exception;
//        }

        jobject json_object = env->NewObject(json_object_class, json_object_constructor, json_j_string);
//        exception = throw_jvm_exception_in_js(env, ctx);
//        if (JS_IsException(exception)) {
//            return exception;
//        }

        jfieldID args_field = env->GetFieldID(java_function_class, "args", "Lorg/json/JSONObject;");
        env->SetObjectField(java_function_obj, args_field, json_object);
    }

    env->CallVoidMethod(java_function_obj, java_method);
//    exception = throw_jvm_exception_in_js(env, ctx);
//    if (JS_IsException(exception)) {
//        return exception;
//    }

    return JS_UNDEFINED;
}

std::string NativeAndroidInterface::crypto_get_random_uuid() {
    return "";
}

std::vector<uint8_t> NativeAndroidInterface::crypto_get_random(size_t size) {
    return std::vector<uint8_t>();
}

int NativeAndroidInterface::get_random_hash() {
    return 0;
}

NativeResponse NativeAndroidInterface::fetch(NativeRequest request) {
    NativeResponse native_response;

    return native_response;
}

std::string NativeAndroidInterface::byte_array_to_str(uint8_t* arr, const std::string& encoding) {
    return "";
}

std::vector<uint8_t> NativeAndroidInterface::string_to_byte_array(std::string str) {
    return std::vector<uint8_t>();
}
