#ifndef ANDROID_JS_ENGINE_API_KV_H
#define ANDROID_JS_ENGINE_API_KV_H

#include "../quickjs/quickjs.h"

JSValue api_kv_set(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);
JSValue api_kv_get(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);
JSValue api_kv_remove(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);

#endif //ANDROID_JS_ENGINE_API_KV_H
