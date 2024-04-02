#ifndef CAPACITOR_BACKGROUND_RUNNER_API_CONSOLE_H
#define CAPACITOR_BACKGROUND_RUNNER_API_CONSOLE_H

#include <android/log.h>
#include "../quickjs/quickjs.h"

void write_to_logcat(android_LogPriority priority, const char *tag, const char *message);

JSValue api_console_log(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);
JSValue api_console_warn(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);
JSValue api_console_error(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);
JSValue api_console_debug(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);

#endif //CAPACITOR_BACKGROUND_RUNNER_API_CONSOLE_H
