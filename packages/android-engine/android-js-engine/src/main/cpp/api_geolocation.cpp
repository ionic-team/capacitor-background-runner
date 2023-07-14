#include "api_geolocation.h"
#include "context.h"

JSValue api_geolocation_current_location(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    JSValue ret_value = JS_UNDEFINED;

    auto *parent_ctx = (Context *)JS_GetContextOpaque(ctx);
    auto *env = parent_ctx->getJNIEnv();

    auto *geolocation = env->GetObjectField(parent_ctx->capacitor_api, parent_ctx->jni_classes->capacitor_api_geolocation_field);

    jclass j_geolocation_class = env->FindClass("io/ionic/android_js_engine/api/Geolocation");
    jmethodID j_geolocation_current_location_method = env->GetMethodID(j_geolocation_class, "getCurrentPosition", "()Ljava/lang/String;");

    auto value_j_str = (jstring)env->CallObjectMethod(geolocation, j_geolocation_current_location_method);
    if (value_j_str == nullptr) {
        return JS_NULL;
    }

    auto value_c_str = env->GetStringUTFChars(value_j_str, nullptr);

    ret_value = JS_NewString(ctx, value_c_str);

    env->ReleaseStringUTFChars(value_j_str, value_c_str);

    return ret_value;
}
