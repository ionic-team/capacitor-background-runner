#ifndef CAPACITOR_BACKGROUND_RUNNER_JNI_CLASSES_H
#define CAPACITOR_BACKGROUND_RUNNER_JNI_CLASSES_H

#include <jni.h>

class JNIClasses {
public:
    jclass context_api_class;
    jclass js_response_class;

    jmethodID context_api_fetch_method;
    jmethodID context_api_byteArrayToString_method;

    jfieldID js_response_ok_field;
    jfieldID js_response_status_field;
    jfieldID js_response_url_field;
    jfieldID js_response_data_field;
    jfieldID js_response_error_field;

    JNIClasses(JNIEnv *env);

private:
    void check_exception(JNIEnv *env);
};

#endif //CAPACITOR_BACKGROUND_RUNNER_JNI_CLASSES_H
