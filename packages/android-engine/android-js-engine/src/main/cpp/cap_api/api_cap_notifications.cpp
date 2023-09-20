#include "api_cap_notifications.h"
#include "../context.h"
#include "../errors.h"

JSValue api_notifications_schedule(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    auto *context = (Context *)JS_GetContextOpaque(ctx);
    auto *env = context->java->getEnv();

    auto *notification = env->GetObjectField(context->cap_api, context->java->capacitor_api_notification_field);
    auto exception = throw_jvm_exception_in_js(env, ctx);
    if (JS_IsException(exception)) {
        return exception;
    }

    auto options_str = JS_JSONStringify(ctx, argv[0], JS_UNDEFINED, JS_UNDEFINED);
    const auto *options_c_str = JS_ToCString(ctx, options_str);
    auto *options_j_str = env->NewStringUTF(options_c_str);

    env->CallVoidMethod(notification, context->java->capacitor_api_notifications_schedule_method, options_j_str);

    JS_FreeCString(ctx, options_c_str);
    JS_FreeValue(ctx, options_str);

    exception = throw_jvm_exception_in_js(env, ctx);
    if (JS_IsException(exception)) {
        return exception;
    }

    return JS_UNDEFINED;
}
