#include "java.h"

#include <stdexcept>

Java::Java(JNIEnv *env) {
    this->vm = nullptr;
    env->GetJavaVM(&this->vm);

    jclass tmp_class;

    // Context API
    tmp_class = env->FindClass("io/ionic/android_js_engine/NativeWebAPI");
    this->check_exception(env);

    this->web_api_class = (jclass)env->NewGlobalRef(tmp_class);
    env->DeleteLocalRef(tmp_class);
    tmp_class = nullptr;

    tmp_class = env->FindClass("io/ionic/android_js_engine/NativeJSFetchOptions");
    this->check_exception(env);

    this->native_js_fetch_options_class = (jclass)env->NewGlobalRef(tmp_class);
    env->DeleteLocalRef(tmp_class);
    tmp_class = nullptr;

    tmp_class = env->FindClass("io/ionic/android_js_engine/NativeJSResponse");
    this->check_exception(env);

    this->native_js_response_class = (jclass)env->NewGlobalRef(tmp_class);
    env->DeleteLocalRef(tmp_class);
    tmp_class = nullptr;

    this->web_api_byteArrayToString_method = env->GetStaticMethodID(this->web_api_class, "byteArrayToString", "([BLjava/lang/String;)Ljava/lang/String;");
    this->check_exception(env);

    this->web_api_stringToByteArray_method = env->GetStaticMethodID(this->web_api_class, "stringToByteArray", "(Ljava/lang/String;)[B");
    this->check_exception(env);

    this->web_api_randomHashCode_method = env->GetStaticMethodID(this->web_api_class, "randomHashCode", "()I");
    this->check_exception(env);

    this->web_api_cryptoGetRandom_method = env->GetStaticMethodID(this->web_api_class, "cryptoGetRandom", "(I)[B");
    this->check_exception(env);

    this->web_api_cryptoRandomUUID_method = env->GetStaticMethodID(this->web_api_class, "cryptoRandomUUID", "()Ljava/lang/String;");
    this->check_exception(env);

    this->web_api_fetch_method = env->GetStaticMethodID(this->web_api_class, "fetch", "(Ljava/lang/String;Lio/ionic/android_js_engine/NativeJSFetchOptions;)Lio/ionic/android_js_engine/NativeJSResponse;");
    this->check_exception(env);

    this->native_js_fetch_options_constructor = env->GetMethodID(this->native_js_fetch_options_class, "<init>", "(Ljava/lang/String;Ljava/util/HashMap;[B)V");
    this->check_exception(env);

    this->native_js_response_ok_field = env->GetFieldID(this->native_js_response_class, "ok", "Z");
    this->check_exception(env);

    this->native_js_response_status_field = env->GetFieldID(this->native_js_response_class, "status", "I");
    this->check_exception(env);

    this->native_js_response_url_field = env->GetFieldID(this->native_js_response_class, "url", "Ljava/lang/String;");
    this->check_exception(env);

    this->native_js_response_data_field = env->GetFieldID(this->native_js_response_class, "data", "[B");
    this->check_exception(env);

    this->native_js_response_error_field = env->GetFieldID(this->native_js_response_class, "error", "Ljava/lang/String;");
    this->check_exception(env);
}

JNIEnv *Java::getEnv() {
    JNIEnv *env = nullptr;

    int status = this->vm->GetEnv((void **)&env, JNI_VERSION_1_6);
    if (status == JNI_EDETACHED) {
        if (this->vm->AttachCurrentThread(&env, NULL) != JNI_OK) {
            return nullptr;
        }
    }

    if (status == JNI_EVERSION) {
        return nullptr;
    }

    return env;
}

void Java::check_exception(JNIEnv *env) {
    if (!env->ExceptionCheck()) {
        return;
    }

    auto *throwable = env->ExceptionOccurred();
    env->ExceptionClear();

    jclass exception_class = env->FindClass("java/lang/Exception");
    jmethodID constructor = env->GetMethodID(exception_class, "<init>", "(Ljava/lang/Throwable;)V");
    jmethodID get_message_method_id = env->GetMethodID(exception_class, "getMessage", "()Ljava/lang/String;");

    auto *err_obj = env->NewObject(exception_class, constructor, throwable);
    auto *err_msg = (jstring)env->CallObjectMethod(err_obj, get_message_method_id);

    const auto *c_err_msg = env->GetStringUTFChars(err_msg, nullptr);

    throw std::runtime_error(c_err_msg);
}