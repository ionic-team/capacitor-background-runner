#ifndef ANDROID_JS_ENGINE_NATIVE_CAPACITOR_INTERFACE_H
#define ANDROID_JS_ENGINE_NATIVE_CAPACITOR_INTERFACE_H

#include "./js-engine/src/capacitor.hpp"
#include "java.h"

#include <string>

class NativeCapacitorInterface : public CapacitorInterface {
public:
    NativeCapacitorInterface(JNIEnv *env, jobject api);
    ~NativeCapacitorInterface();

    jobject api;
    Java *java;

    virtual std::string device_api_getBatteryStatus();
    virtual std::string device_api_getNetworkStatus();

    virtual std::string geolocation_api_getCurrentPosition();

    virtual void kv_api_set(std::string key, std::string json_value);
    virtual std::string kv_api_get(std::string key);
    virtual void kv_api_remove(std::string key);

    virtual void notifications_api_schedule(std::string options_json);
    virtual void notifications_api_setBadge(std::string options_json);
    virtual void notifications_api_clearBadge();

    virtual std::string app_api_getInfo();
    virtual std::string app_api_getState();
};

#endif //ANDROID_JS_ENGINE_NATIVE_CAPACITOR_INTERFACE_H
