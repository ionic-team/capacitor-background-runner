#include <future>
#include <android/log.h>
#include "api_fetch.h"
#include "errors.h"
#include "context.h"

#include <chrono>
#include <thread>



JSValue js_response_json_job(JSContext *ctx, int argc, JSValueConst *argv) {
    JSValue resolve, reject, data;

    resolve = argv[0];
    reject = argv[1];
    data = argv[2];

    uint8_t *buf;
    size_t elem, len, offset, buf_size;
    int size;

    JSValue t_arr = JS_GetTypedArrayBuffer(ctx, data, &offset, &len, &elem);

    size = len;

    buf = JS_GetArrayBuffer(ctx, &buf_size, t_arr);

    auto *parent_ctx = (Context *)JS_GetContextOpaque(ctx);
    auto *env = parent_ctx->getJNIEnv();

    jstring encoding = env->NewStringUTF("utf-8");
    jbyteArray j_byte_array = env->NewByteArray(size);

    env->SetByteArrayRegion(j_byte_array, 0, size, reinterpret_cast<const jbyte *>(buf));

    auto *json_string = (jstring) env->CallObjectMethod(parent_ctx->api, parent_ctx->jni_classes->context_api_byteArrayToString_method, j_byte_array, encoding);

    const char * json_c_string = env->GetStringUTFChars(json_string, nullptr);

    JSValue json = parent_ctx->parseJSON(std::string(json_c_string));

    JSValue global_obj = JS_GetGlobalObject(ctx);
    JS_FreeValue(ctx, JS_Call(ctx, resolve, global_obj, 1, (JSValueConst*)&json));
    JS_FreeValue(ctx, global_obj);
    JS_FreeValue(ctx, t_arr);
    JS_FreeValue(ctx, json);

    env->ReleaseStringUTFChars(json_string, json_c_string);

    return JS_UNDEFINED;
}

JSValue js_response_json_promise(JSContext *ctx, JSValueConst this_val, int argc, JSValue *argv) {
    JSValue promise, resolving_funcs[2];
    JSValueConst args[3];

    promise = JS_NewPromiseCapability(ctx, resolving_funcs);
    if (JS_IsException(promise)) {
        JS_Throw(ctx, promise);
        return JS_UNDEFINED;
    }

    JSValue data = JS_GetPropertyStr(ctx, this_val, "_data");

    args[0] = resolving_funcs[0];
    args[1] = resolving_funcs[1];
    args[2] = data;

    JS_EnqueueJob(ctx, js_response_json_job, 4, args);

    JS_FreeValue(ctx, resolving_funcs[0]);
    JS_FreeValue(ctx, resolving_funcs[1]);
    JS_FreeValue(ctx, data);

    return promise;
}

JSValue js_response_to_value(JSContext *ctx, JNIEnv *env, jobject response) {
    int flags = JS_EVAL_TYPE_GLOBAL | JS_EVAL_FLAG_BACKTRACE_BARRIER;
    uint8_t *buf;
    size_t elem, len, offset, buf_size;

    JSValue jni_exception, js_response;

    auto *parent_ctx = (Context *)JS_GetContextOpaque(ctx);

    auto j_ok = env->GetBooleanField(response, parent_ctx->jni_classes->js_response_ok_field);
    auto j_status = env->GetIntField(response, parent_ctx->jni_classes->js_response_status_field);
    auto *j_url = (jstring) env->GetObjectField(response, parent_ctx->jni_classes->js_response_url_field);
    auto *j_data = static_cast<jbyteArray>(env->GetObjectField(response, parent_ctx->jni_classes->js_response_data_field));
    auto *j_err = (jstring) env->GetObjectField(response, parent_ctx->jni_classes->js_response_error_field);

    const auto *c_url  = env->GetStringUTFChars(j_url, nullptr);

    js_response = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, js_response, "ok", JS_NewBool(ctx, (int)j_ok));
    JS_SetPropertyStr(ctx, js_response, "status", JS_NewInt32(ctx, (int)j_status));
    JS_SetPropertyStr(ctx, js_response, "url", JS_NewString(ctx, c_url));

    if (j_data != nullptr) {
        auto j_data_len = env->GetArrayLength(j_data);
        auto *j_data_arr = env->GetByteArrayElements(j_data, 0);

        std::string new_uint8arr_code = "new Uint8Array(" + std::to_string(j_data_len) + ")";
        JSValue uint8arr = JS_Eval(ctx, new_uint8arr_code.c_str(), new_uint8arr_code.length(), "<internal>", flags);

        JSValue t_arr = JS_GetTypedArrayBuffer(ctx, uint8arr, &offset, &len, &elem);

        buf = JS_GetArrayBuffer(ctx, &buf_size, t_arr);

        for (int i = 0; i < len; i++) {
            buf[i] = j_data_arr[i];
        }

        JS_SetPropertyStr(ctx, js_response, "_data", JS_DupValue(ctx, uint8arr));

        env->ReleaseByteArrayElements(j_data, j_data_arr, 0);
        JS_FreeValue(ctx, t_arr);
        JS_FreeValue(ctx, uint8arr);
    } else {
        JS_SetPropertyStr(ctx, js_response, "_data", JS_NULL);
    }

    if (j_err != nullptr) {
        const auto *c_err = env->GetStringUTFChars(j_err, nullptr);
        JS_SetPropertyStr(ctx, js_response, "error", JS_NewString(ctx, c_err));
        env->ReleaseStringUTFChars(j_err, c_err);
    } else {
        JS_SetPropertyStr(ctx, js_response, "error", JS_NULL);
    }

    env->ReleaseStringUTFChars(j_url, c_url);

    JS_SetPropertyStr(ctx, js_response, "json", JS_NewCFunction(ctx, js_response_json_promise, "json", 0));

    return js_response;
}

JSValue js_fetch_job(JSContext *ctx, int argc, JSValueConst *argv)
{
    JSValue resolve, reject, request, options;
    JSValue jni_exception;

    resolve = argv[0];
    reject = argv[1];
    request = argv[2];
    options = argv[3];

    auto *parent_ctx = (Context *)JS_GetContextOpaque(ctx);

    auto* env = parent_ctx->getJNIEnv();

    JSValue global_obj = JS_GetGlobalObject(ctx);

    if (JS_IsNull(request) || !JS_IsString(request)) {
        JSValue error = JS_NewError(ctx);
        JS_SetPropertyStr(ctx, error, "message", JS_NewString(ctx, "invalid url"));

        JS_Call(ctx, reject, global_obj, 1, (JSValueConst *)&jni_exception);
        JS_FreeValue(ctx, global_obj);
        JS_FreeValue(ctx, error);
        parent_ctx->vm->DetachCurrentThread();

        return JS_UNDEFINED;
    }

    auto c_url_str = JS_ToCString(ctx, request);

    auto j_url_str = env->NewStringUTF(c_url_str);

    jobject j_options = nullptr;

    if (!JS_IsNull(options) && !JS_IsUndefined(options)) {
        auto options_json = parent_ctx->stringifyJSON(options);
        auto j_options_json = env->NewStringUTF(options_json.c_str());

        j_options = env->NewObject(parent_ctx->jni_classes->js_fetch_options_class, parent_ctx->jni_classes->js_fetch_options_constructor, j_options_json);

        jni_exception = check_and_throw_jni_exception(env, ctx);
        if (JS_IsException(jni_exception)) {
            JS_Call(ctx, reject, global_obj, 1, (JSValueConst *)&jni_exception);
            JS_FreeValue(ctx, global_obj);
            env->DeleteLocalRef(j_options_json);

            parent_ctx->vm->DetachCurrentThread();

            return JS_UNDEFINED;
        }

        env->DeleteLocalRef(j_options_json);
    }

    auto response = env->CallObjectMethod(parent_ctx->api, parent_ctx->jni_classes->context_api_fetch_method, j_url_str,
                                          j_options);
    jni_exception = check_and_throw_jni_exception(env, ctx);
    if (JS_IsException(jni_exception)) {
        JS_Call(ctx, reject, global_obj, 1, (JSValueConst *)&jni_exception);
        JS_FreeValue(ctx, global_obj);

        parent_ctx->vm->DetachCurrentThread();

        return JS_UNDEFINED;
    }

    auto res = js_response_to_value(ctx, env, response);

    JSValue err = JS_GetPropertyStr(ctx, res, "error");
    if (JS_IsString(err)) {
        JS_Call(ctx, reject, global_obj, 1, (JSValueConst *)&err);
        JS_FreeValue(ctx, global_obj);
        JS_FreeValue(ctx, res);
        JS_FreeValue(ctx, err);
        return JS_UNDEFINED;
    }

    JS_Call(ctx, resolve, global_obj, 1, (JSValueConst *)&res);

    JS_FreeValue(ctx, global_obj);
    JS_FreeValue(ctx, res);
    JS_FreeValue(ctx, err);

    parent_ctx->vm->DetachCurrentThread();

    return JS_UNDEFINED;
}

JSValue api_fetch_promise(JSContext *ctx, JSValueConst this_val, int argc, JSValue *argv) {
    JSValue promise, resolving_funcs[2];
    JSValueConst args[4];

    promise = JS_NewPromiseCapability(ctx, resolving_funcs);
    if (JS_IsException(promise)) {
        JS_Throw(ctx, promise);
        return JS_UNDEFINED;
    }

    args[0] = resolving_funcs[0];
    args[1] = resolving_funcs[1];
    args[2] = argv[0];
    args[3] = argv[1];

    JS_EnqueueJob(ctx, js_fetch_job, 4, args);

    JS_FreeValue(ctx, resolving_funcs[0]);
    JS_FreeValue(ctx, resolving_funcs[1]);

    return promise;
}
