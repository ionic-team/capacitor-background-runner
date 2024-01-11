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