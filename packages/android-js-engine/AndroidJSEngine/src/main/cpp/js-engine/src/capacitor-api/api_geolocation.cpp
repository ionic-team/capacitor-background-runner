#include "api_geolocation.h"
#include "../context.hpp"


JSValue api_geolocation_current_location(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    auto *context = (Context *)JS_GetContextOpaque(ctx);

    try {
        auto json_result = context->capacitor_interface->geolocation_api_getCurrentPosition();
        if (json_result.empty()) {
            return JS_NULL;
        }

        return JS_ParseJSON(ctx, json_result.c_str(), strlen(json_result.c_str()), "<parse geolocation status>");
    } catch(std::exception& ex) {
        auto js_error = create_js_error(ex.what(), ctx);
        return JS_Throw(ctx, js_error);
    }
}
