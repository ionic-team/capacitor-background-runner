#ifndef ANDROID_JS_ENGINE_API_APP_H
#define ANDROID_JS_ENGINE_API_APP_H

#include "../quickjs/quickjs.h"

JSValue api_app_get_info(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);
JSValue api_app_get_state(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);
JSValue api_app_set_badge(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);
JSValue api_app_get_badge(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);
JSValue api_app_clear_badge(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);

#endif //ANDROID_JS_ENGINE_API_APP_H
