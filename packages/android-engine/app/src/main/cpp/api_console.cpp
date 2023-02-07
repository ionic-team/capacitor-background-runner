#include "api_console.h"

#include <android/log.h>

void write_to_logcat(android_LogPriority priority, const char * tag, const char * message) {
    __android_log_write(priority, tag, message);
}

JSValue api_console_log(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int i;
    const char *str;
    size_t len;

    for(i = 0; i < argc; i++) {
        str = JS_ToCStringLen(ctx, &len, argv[i]);
        if (!str) {
            return JS_EXCEPTION;
        }

        write_to_logcat(ANDROID_LOG_INFO, "[JS CONTEXT]", str);

        JS_FreeCString(ctx, str);
    }

    return JS_UNDEFINED;
};

JSValue api_console_warn(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int i;
    const char *str;
    size_t len;

    for(i = 0; i < argc; i++) {
        str = JS_ToCStringLen(ctx, &len, argv[i]);
        if (!str) {
            return JS_EXCEPTION;
        }

        write_to_logcat(ANDROID_LOG_WARN, "[JS CONTEXT]", str);

        JS_FreeCString(ctx, str);
    }

    return JS_UNDEFINED;
};

JSValue api_console_error(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int i;
    const char *str;
    size_t len;

    for(i = 0; i < argc; i++) {
        str = JS_ToCStringLen(ctx, &len, argv[i]);
        if (!str) {
            return JS_EXCEPTION;
        }

        write_to_logcat(ANDROID_LOG_ERROR, "[JS CONTEXT]", str);

        JS_FreeCString(ctx, str);
    }

    return JS_UNDEFINED;
};

JSValue api_console_debug(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    int i;
    const char *str;
    size_t len;

    for(i = 0; i < argc; i++) {
        str = JS_ToCStringLen(ctx, &len, argv[i]);
        if (!str) {
            return JS_EXCEPTION;
        }

        write_to_logcat(ANDROID_LOG_DEBUG, "[JS CONTEXT]", str);

        JS_FreeCString(ctx, str);
    }

    return JS_UNDEFINED;
}
