#ifndef CAPACITOR_BACKGROUND_RUNNER_API_GEOLOCATION_H
#define CAPACITOR_BACKGROUND_RUNNER_API_GEOLOCATION_H

#include "quickjs/quickjs.h"

JSValue api_geolocation_current_location(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);

#endif //CAPACITOR_BACKGROUND_RUNNER_API_GEOLOCATION_H
