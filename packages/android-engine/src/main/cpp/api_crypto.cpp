#include "api_crypto.h"
#include "Context.h"

JSValue api_crypto_get_random_values(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    JSValue ret_value = JS_UNDEFINED;

    uint8_t *buf;
    size_t elem, len, offset, buf_size;
    int size;

    JSValue t_arr = JS_GetTypedArrayBuffer(ctx, argv[0], &offset, &len, &elem);

    size = len;

    buf = JS_GetArrayBuffer(ctx, &buf_size, t_arr);

    if (!buf) {
        return JS_EXCEPTION;
    }

    // TODO: Check for JNI Exceptions
    Context *parent_ctx = (Context *)JS_GetContextOpaque(ctx);

    jclass j_context_class = parent_ctx->env->FindClass("io/ionic/backgroundrunner/Context");
    jmethodID j_method = parent_ctx->env->GetStaticMethodID(j_context_class, "cryptoGetRandom", "(I)[B");

    jbyteArray random_bytes = static_cast<jbyteArray>(parent_ctx->env->CallStaticObjectMethod(j_context_class, j_method, size));
    auto random = parent_ctx->env->GetByteArrayElements(random_bytes, 0);

    for (int i = 0; i < size; i++) {
        buf[i] = random[i];
    }

    parent_ctx->env->ReleaseByteArrayElements(random_bytes, random, 0);

    JS_FreeValue(ctx, t_arr);

    ret_value = JS_DupValue(ctx, argv[0]);

    return ret_value;
}

JSValue api_crypto_random_uuid(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    // TODO: Check for JNI Exceptions

    JSValue ret_value = JS_UNDEFINED;

    Context *parent_ctx = (Context *)JS_GetContextOpaque(ctx);

    jclass j_context_class = parent_ctx->env->FindClass("io/ionic/backgroundrunner/Context");
    jmethodID j_method = parent_ctx->env->GetStaticMethodID(j_context_class, "cryptoRandomUUID", "()Ljava/lang/String;");

    jstring str = (jstring) parent_ctx->env->CallStaticObjectMethod(j_context_class, j_method);

    auto c_str = parent_ctx->env->GetStringUTFChars(str, 0);

    ret_value = JS_NewString(ctx, c_str);

    parent_ctx->env->ReleaseStringUTFChars(str, c_str);

    return ret_value;
}
