#include "api_device.h"

#include "context.h"
#include "errors.h"

JSValue api_device_battery(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    JSValue ret_value, jni_exception = JS_UNDEFINED;

    auto *parent_ctx = (Context *)JS_GetContextOpaque(ctx);
    auto *env = parent_ctx->getJNIEnv();

    auto *device = env->GetObjectField(parent_ctx->capacitor_api, parent_ctx->jni_classes->capacitor_api_device_field);

    jmethodID j_device_battery_method = env->GetMethodID(parent_ctx->jni_classes->device_api_class, "getBatteryStatus", "()Ljava/lang/String;");

    auto *j_json_str = (jstring)env->CallObjectMethod(device, j_device_battery_method);
    jni_exception = check_and_throw_jni_exception(env, ctx);
    if (JS_IsException(jni_exception)) {
        return jni_exception;
    }

    if (j_json_str == nullptr) {
        return JS_NULL;
    }

    const auto *c_json_str = env->GetStringUTFChars(j_json_str, nullptr);

    ret_value = parent_ctx->parseJSON(c_json_str);

    env->ReleaseStringUTFChars(j_json_str, c_json_str);

    return ret_value;
}

JSValue api_device_network(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    JSValue ret_value, jni_exception = JS_UNDEFINED;

    auto *parent_ctx = (Context *)JS_GetContextOpaque(ctx);
    auto *env = parent_ctx->getJNIEnv();

    auto *device = env->GetObjectField(parent_ctx->capacitor_api, parent_ctx->jni_classes->capacitor_api_device_field);

    jmethodID j_device_network_method = env->GetMethodID(parent_ctx->jni_classes->device_api_class, "getNetworkStatus", "()Ljava/lang/String;");

    auto *j_json_str = (jstring)env->CallObjectMethod(device, j_device_network_method);
    jni_exception = check_and_throw_jni_exception(env, ctx);
    if (JS_IsException(jni_exception)) {
        return jni_exception;
    }

    if (j_json_str == nullptr) {
        return JS_NULL;
    }

    const auto *c_json_str = env->GetStringUTFChars(j_json_str, nullptr);

    ret_value = parent_ctx->parseJSON(c_json_str);

    env->ReleaseStringUTFChars(j_json_str, c_json_str);

    return ret_value;
}
