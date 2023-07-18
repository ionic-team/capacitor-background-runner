#include <android/log.h>

#include "quickjs/quickjs.h"

#ifndef ANDROID_ENGINE_API_CONSOLE_H
#define ANDROID_ENGINE_API_CONSOLE_H

void write_to_logcat(android_LogPriority priority, const char *tag, const char *message);

JSValue api_console_log(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);
JSValue api_console_warn(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);
JSValue api_console_error(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);
JSValue api_console_debug(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);

#endif //ANDROID_ENGINE_API_CONSOLE_H
