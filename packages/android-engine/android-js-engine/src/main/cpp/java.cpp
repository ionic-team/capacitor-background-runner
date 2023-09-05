#include "java.h"

Java::Java(JNIEnv *env) {
    this->vm = nullptr;
    env->GetJavaVM(&this->vm);


}

JNIEnv* Java::getEnv() {
    JNIEnv *env = nullptr;

    int status = this->vm->GetEnv((void **)&env, JNI_VERSION_1_6);
    if (status == JNI_EDETACHED) {
        if (this->vm->AttachCurrentThread(&env, NULL) != JNI_OK) {
            // TODO: throw exception here
            return nullptr;
        }
    }

    if (status == JNI_EVERSION) {
        // TODO: throw exception here
        return nullptr;
    }

    return env;
}
