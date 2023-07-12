#include <stdexcept>
#include "jni_classes.h"

JNIClasses::JNIClasses(JNIEnv *env)
{
    // get all needed JNI classes and methods

    // Context API
    this->context_api_class = env->FindClass("io/ionic/android_js_engine/ContextAPI");
    this->check_exception(env);

    this->js_response_class = env->FindClass("io/ionic/android_js_engine/JSResponse");
    this->check_exception(env);

    this->context_api_fetch_method = env->GetMethodID(this->context_api_class, "fetch", "(Ljava/lang/String;)Lio/ionic/android_js_engine/JSResponse;");
    this->check_exception(env);

    this->js_response_ok_field = env->GetFieldID(this->js_response_class, "ok", "Z");
    this->check_exception(env);

    this->js_response_status_field = env->GetFieldID(this->js_response_class, "status", "I");
    this->check_exception(env);

    this->js_response_url_field = env->GetFieldID(this->js_response_class, "url", "Ljava/lang/String;");
    this->check_exception(env);

    this->js_response_data_field = env->GetFieldID(this->js_response_class, "data", "[B");
    this->check_exception(env);
}

void JNIClasses::check_exception(JNIEnv *env)
{
    if (!env->ExceptionCheck()) {
        return;
    }

    auto throwable = env->ExceptionOccurred();
    env->ExceptionClear();

    jclass c = env->FindClass("java/lang/Exception");
    jmethodID cnstrctr = env->GetMethodID(c, "<init>", "(Ljava/lang/Throwable;)V");
    jmethodID getMessage = env->GetMethodID(c, "getMessage", "()Ljava/lang/String;");

    auto errObj = env->NewObject(c, cnstrctr, throwable);
    auto errMsg = (jstring) env->CallObjectMethod(errObj, getMessage);

    auto c_errMsg = env->GetStringUTFChars(errMsg, nullptr);

    throw std::runtime_error(c_errMsg);
}
