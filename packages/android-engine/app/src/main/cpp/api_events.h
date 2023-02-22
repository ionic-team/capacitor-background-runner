#include "quickjs/quickjs.h"

#ifndef ANDROID_ENGINE_API_EVENTS_H
#define ANDROID_ENGINE_API_EVENTS_H

JSValue api_add_event_listener(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);

#endif //ANDROID_ENGINE_API_EVENTS_H
