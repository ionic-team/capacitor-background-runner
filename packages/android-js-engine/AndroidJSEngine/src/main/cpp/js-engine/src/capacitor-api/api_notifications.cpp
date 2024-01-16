#include "api_notifications.h"
#include "../context.hpp"

JSValue api_notifications_schedule(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    auto *context = (Context *)JS_GetContextOpaque(ctx);
    auto options_str = JS_JSONStringify(ctx, argv[0], JS_UNDEFINED, JS_UNDEFINED);
    const auto *options_c_str = JS_ToCString(ctx, options_str);

    try {
        context->capacitor_interface->notifications_api_schedule(options_c_str);

        JS_FreeCString(ctx, options_c_str);

        return JS_UNDEFINED;
    } catch(std::exception& ex) {
        JS_FreeCString(ctx, options_c_str);

        auto js_error = create_js_error(ex.what(), ctx);
        return JS_Throw(ctx, js_error);
    }
}