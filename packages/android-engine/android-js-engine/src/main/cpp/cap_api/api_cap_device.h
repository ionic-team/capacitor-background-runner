#ifndef ANDROID_API_CAP_DEVICE_H
#define ANDROID_API_CAP_DEVICE_H

#include "../quickjs/quickjs.h"

JSValue api_device_battery(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);
JSValue api_device_network(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);

#endif //ANDROID_API_CAP_DEVICE_H
