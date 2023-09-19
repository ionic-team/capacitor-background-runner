#ifndef CAPACITOR_BACKGROUND_RUNNER_JAVA_H
#define CAPACITOR_BACKGROUND_RUNNER_JAVA_H

#include <jni.h>

class Java {
public:
    JavaVM *vm;

    jclass web_api_class;
    jclass js_response_class;
    jclass js_fetch_options_class;
    jclass capacitor_api_class;
    jclass capacitor_kv_api_class;
    jclass capacitor_notification_api_class;
    jclass capacitor_device_api_class;
    jclass capacitor_geolocation_api_class;

    jmethodID web_api_fetch_method;
    jmethodID web_api_byteArrayToString_method;
    jmethodID web_api_stringToByteArray_method;
    jmethodID web_api_randomHashCode_method;
    jmethodID web_api_cryptoRandomUUID_method;
    jmethodID web_api_cryptoGetRandom_method;
    jmethodID js_fetch_options_constructor;

    jfieldID js_response_ok_field;
    jfieldID js_response_status_field;
    jfieldID js_response_url_field;
    jfieldID js_response_data_field;
    jfieldID js_response_error_field;
    jfieldID capacitor_api_kv_field;
    jfieldID capacitor_api_device_field;
    jfieldID capacitor_api_notification_field;
    jfieldID capacitor_api_geolocation_field;

    Java(JNIEnv *env);
    JNIEnv *getEnv();

private:
    void check_exception(JNIEnv *env);
};

#endif //CAPACITOR_BACKGROUND_RUNNER_JAVA_H
