#ifndef ANDROID_JS_ENGINE_API_NOTIFICATIONS_H
#define ANDROID_JS_ENGINE_API_NOTIFICATIONS_H

#include "../quickjs/quickjs.h"

JSValue api_notifications_schedule(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);

#endif //ANDROID_JS_ENGINE_API_NOTIFICATIONS_H
