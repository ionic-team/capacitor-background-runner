#include "api_crypto.h"
#include "../context.h"
#include "../errors.h"

JSValue api_crypto_get_random_values(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    uint8_t *buf;
    size_t elem, len, offset, buf_size;
    int size;

    auto t_arr_buf = JS_GetTypedArrayBuffer(ctx, argv[0], &offset, &len, &elem);
    buf = JS_GetArrayBuffer(ctx, &buf_size, t_arr_buf);

    if (!buf) {
        return JS_EXCEPTION;
    }

    size = buf_size;

    auto *context = (Context *)JS_GetContextOpaque(ctx);

    auto *env = context->java->getEnv();
    if (env == nullptr) {
        return throw_js_exception(ctx, "JVM Environment is null");
    }

    auto random_bytes = static_cast<jbyteArray>(env->CallStaticObjectMethod(context->java->web_api_class, context->java->web_api_cryptoGetRandom_method, size));
    auto exception = throw_jvm_exception_in_js(env, ctx);
    if (JS_IsException(exception)) {
        return exception;
    }

    auto random = env->GetByteArrayElements(random_bytes, nullptr);

    for (int i = 0; i < size; i++) {
        buf[i] = random[i];
    }

    env->ReleaseByteArrayElements(random_bytes, random, 0);

    JS_FreeValue(ctx, t_arr_buf);

    return JS_DupValue(ctx, argv[0]);
}

JSValue api_crypto_random_uuid(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    auto *context = (Context *)JS_GetContextOpaque(ctx);

    auto *env = context->java->getEnv();
    if (env == nullptr) {
        return throw_js_exception(ctx, "JVM Environment is null");
    }

    auto *str = (jstring)env->CallStaticObjectMethod(context->java->web_api_class, context->java->web_api_cryptoRandomUUID_method);
    auto exception = throw_jvm_exception_in_js(env, ctx);
    if (JS_IsException(exception)) {
        return exception;
    }

    const auto *c_str = env->GetStringUTFChars(str, nullptr);

    auto ret_value = JS_NewString(ctx, c_str);

    env->ReleaseStringUTFChars(str, c_str);

    return ret_value;
}