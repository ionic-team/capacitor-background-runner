#include "api_cap_device.h"
#include "../context.h"
#include "../errors.h"

JSValue api_device_battery(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    auto *context = (Context *)JS_GetContextOpaque(ctx);
    auto *env = context->java->getEnv();

    auto *device = env->GetObjectField(context->cap_api, context->java->capacitor_api_device_field);
    auto exception = throw_jvm_exception_in_js(env, ctx);
    if (JS_IsException(exception)) {
        return exception;
    }

    auto *j_json_str = (jstring)env->CallObjectMethod(device, context->java->capacitor_api_device_getBatteryStatus_method);
    exception = throw_jvm_exception_in_js(env, ctx);
    if (JS_IsException(exception)) {
        return exception;
    }

    if (j_json_str == nullptr) {
        return JS_NULL;
    }

    const auto *c_json_str = env->GetStringUTFChars(j_json_str, nullptr);

    auto ret_value = JS_ParseJSON(ctx, c_json_str, strlen(c_json_str), "<parse battery status>");
    env->ReleaseStringUTFChars(j_json_str, c_json_str);

    return ret_value;
}

JSValue api_device_network(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    auto *context = (Context *)JS_GetContextOpaque(ctx);
    auto *env = context->java->getEnv();

    auto *device = env->GetObjectField(context->cap_api, context->java->capacitor_api_device_field);
    auto exception = throw_jvm_exception_in_js(env, ctx);
    if (JS_IsException(exception)) {
        return exception;
    }

    auto *j_json_str = (jstring)env->CallObjectMethod(device, context->java->capacitor_api_device_getNetworkStatus_method);
    exception = throw_jvm_exception_in_js(env, ctx);
    if (JS_IsException(exception)) {
        return exception;
    }

    if (j_json_str == nullptr) {
        return JS_NULL;
    }

    const auto *c_json_str = env->GetStringUTFChars(j_json_str, nullptr);

    auto ret_value = JS_ParseJSON(ctx, c_json_str, strlen(c_json_str), "<parse network status>");
    env->ReleaseStringUTFChars(j_json_str, c_json_str);

    return ret_value;
}

