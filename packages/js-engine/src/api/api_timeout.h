#ifndef CAPACITOR_BACKGROUND_RUNNER_API_TIMEOUT_H
#define CAPACITOR_BACKGROUND_RUNNER_API_TIMEOUT_H

#include <chrono>
#include "../quickjs/quickjs.h"

struct Timer {
    int timeout;
    std::chrono::time_point<std::chrono::system_clock> start;
    JSValue js_func;
    bool repeat;
};

JSValue api_set_timeout(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);
JSValue api_clear_timeout(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);
JSValue api_set_interval(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);

#endif //CAPACITOR_BACKGROUND_RUNNER_API_TIMEOUT_H
