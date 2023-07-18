#include <chrono>
#include "quickjs/quickjs.h"

#ifndef ANDROID_ENGINE_API_TIMEOUT_H
#define ANDROID_ENGINE_API_TIMEOUT_H

struct Timer {
    int timeout;
    std::chrono::time_point<std::chrono::system_clock> start;
    JSValue js_func;
    bool repeat;
};

JSValue api_set_timeout(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);
JSValue api_clear_timeout(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);
JSValue api_set_interval(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);

#endif //ANDROID_ENGINE_API_TIMEOUT_H
