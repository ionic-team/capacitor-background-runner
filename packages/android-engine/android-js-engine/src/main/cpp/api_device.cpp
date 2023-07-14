#include "api_device.h"

#include "context.h"

JSValue api_device_battery(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    JSValue ret_value = JS_UNDEFINED;

    auto *parent_ctx = (Context *)JS_GetContextOpaque(ctx);
    auto *env = parent_ctx->getJNIEnv();

    auto *device = env->GetObjectField(parent_ctx->capacitor_api, parent_ctx->jni_classes->capacitor_api_device_field);

    jclass j_device_class = env->FindClass("io/ionic/android_js_engine/api/Device");
    jmethodID j_device_battery_method = env->GetMethodID(j_device_class, "getBatteryStatus", "()Ljava/lang/String;");

    auto *j_json_str = (jstring)env->CallObjectMethod(device, j_device_battery_method);
    const auto *c_json_str = env->GetStringUTFChars(j_json_str, nullptr);

    ret_value = parent_ctx->parseJSON(c_json_str);

    env->ReleaseStringUTFChars(j_json_str, c_json_str);

    return ret_value;
}
