#include "api_cap_geolocation.h"
#include "../context.h"
#include "../errors.h"

JSValue api_geolocation_current_location(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    auto *context = (Context *)JS_GetContextOpaque(ctx);
    auto *env = context->java->getEnv();

    auto *geolocation = env->GetObjectField(context->cap_api, context->java->capacitor_api_geolocation_field);
    auto exception = throw_jvm_exception_in_js(env, ctx);
    if (JS_IsException(exception)) {
        return exception;
    }

    auto value_j_str = (jstring)env->CallObjectMethod(geolocation, context->java->capacitor_api_geolocation_getCurrentPosition_method);
    exception = throw_jvm_exception_in_js(env, ctx);
    if (JS_IsException(exception)) {
        return exception;
    }

    if (value_j_str == nullptr) {
        return JS_NULL;
    }

    const auto *c_json_str = env->GetStringUTFChars(value_j_str, nullptr);

    auto ret_value = JS_ParseJSON(ctx, c_json_str, strlen(c_json_str), "<parse geolocation response>");
    env->ReleaseStringUTFChars(value_j_str, c_json_str);

    return ret_value;
}
