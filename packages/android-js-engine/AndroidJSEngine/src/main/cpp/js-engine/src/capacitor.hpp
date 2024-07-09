#ifndef CAPACITOR_BACKGROUND_CAPACITOR_H
#define CAPACITOR_BACKGROUND_CAPACITOR_H

#include "quickjs/quickjs.h"
#include <string>

class CapacitorInterface {
public:
    virtual ~CapacitorInterface() = default;
    CapacitorInterface(const CapacitorInterface&) = delete;
    CapacitorInterface(CapacitorInterface&&) = delete;
    CapacitorInterface& operator=(const CapacitorInterface&) = delete;

    virtual std::string device_api_getBatteryStatus() = 0;
    virtual std::string device_api_getNetworkStatus() = 0;

    virtual std::string geolocation_api_getCurrentPosition() = 0;

    virtual void kv_api_set(std::string key, std::string json_value) = 0;
    virtual std::string kv_api_get(std::string key) = 0;
    virtual void kv_api_remove(std::string key) = 0;

    virtual void notifications_api_schedule(std::string options_json) = 0;
    virtual void notifications_api_setBadge(std::string options_json) = 0;
    virtual void notifications_api_clearBadge() = 0;

    virtual std::string app_api_getInfo() = 0;
    virtual std::string app_api_getState() = 0;
protected:
    CapacitorInterface() = default;
};

#endif //CAPACITOR_BACKGROUND_CAPACITOR_H
