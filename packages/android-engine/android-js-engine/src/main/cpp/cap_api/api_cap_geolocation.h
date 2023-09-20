#ifndef ANDROID_API_CAP_GEOLOCATION_H
#define ANDROID_API_CAP_GEOLOCATION_H

#include "../quickjs/quickjs.h"

JSValue api_geolocation_current_location(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);

#endif //ANDROID_API_CAP_GEOLOCATION_H
