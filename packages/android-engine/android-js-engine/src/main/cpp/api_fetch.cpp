#include <future>
#include <android/log.h>
#include "api_fetch.h"
#include "errors.h"
#include "context.h"

#include <chrono>
#include <thread>


JSValue js_response_to_value(JSContext *ctx, jobject response) {
    JSValue jni_exception, js_response;

    auto *parent_ctx = (Context *)JS_GetContextOpaque(ctx);

    jclass j_response_class = parent_ctx->env->FindClass("io/ionic/android_js_engine/JSResponse");
    jni_exception = check_and_throw_jni_exception(parent_ctx->env, ctx);
    if (JS_IsException(jni_exception)) {
        return jni_exception;
    }

    auto j_ok_field = parent_ctx->env->GetFieldID(j_response_class, "ok", "Z");
    jni_exception = check_and_throw_jni_exception(parent_ctx->env, ctx);
    if (JS_IsException(jni_exception)) {
        return jni_exception;
    }

    auto j_status_field = parent_ctx->env->GetFieldID(j_response_class, "status", "I");
    jni_exception = check_and_throw_jni_exception(parent_ctx->env, ctx);
    if (JS_IsException(jni_exception)) {
        return jni_exception;
    }

    auto j_url_field = parent_ctx->env->GetFieldID(j_response_class, "url", "Ljava/lang/String;");
    jni_exception = check_and_throw_jni_exception(parent_ctx->env, ctx);
    if (JS_IsException(jni_exception)) {
        return jni_exception;
    }

    auto j_data_field = parent_ctx->env->GetFieldID(j_response_class, "data", "[B");
    jni_exception = check_and_throw_jni_exception(parent_ctx->env, ctx);
    if (JS_IsException(jni_exception)) {
        return jni_exception;
    }

    auto j_ok = parent_ctx->env->GetBooleanField(response, j_ok_field);
    auto j_status = parent_ctx->env->GetIntField(response, j_status_field);
    auto j_url = (jstring) parent_ctx->env->GetObjectField(response, j_url_field);
    auto j_data = static_cast<jbyteArray>(parent_ctx->env->GetObjectField(response, j_data_field));
    auto j_data_len = parent_ctx->env->GetArrayLength(j_data);
    auto j_data_arr = parent_ctx->env->GetByteArrayElements(j_data, 0);

    auto c_url  = parent_ctx->env->GetStringUTFChars(j_url, nullptr);

    js_response = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, js_response, "ok", JS_NewBool(ctx, (int)j_ok));
    JS_SetPropertyStr(ctx, js_response, "status", JS_NewInt32(ctx, (int)j_status));
    JS_SetPropertyStr(ctx, js_response, "url", JS_NewString(ctx, c_url));
    JS_SetPropertyStr(ctx, js_response, "_data", JS_NewArrayBuffer(ctx, (uint8_t *)j_data_arr, j_data_len,
                                                                   nullptr, nullptr, 0));

    parent_ctx->env->ReleaseStringUTFChars(j_url, c_url);
    parent_ctx->env->ReleaseByteArrayElements(j_data, j_data_arr, 0);

    return js_response;
}

JSValue fetch_resolve(JSContext *ctx, JSValueConst this_val, int argc, JSValue *argv) {

}

JSValue fetch_reject(JSContext *ctx, JSValueConst this_val, int argc, JSValue *argv) {

}

JSValue js_fetch_job(JSContext *ctx, int argc, JSValueConst *argv)
{
    JSValue resolve, reject;

    resolve = argv[0];
    reject = argv[1];

    JSValue global_obj = JS_GetGlobalObject(ctx);

    JS_Call(ctx, resolve, global_obj, 0, nullptr);

    JS_FreeValue(ctx, global_obj);

    return JS_NewString(ctx, "Hello world!");
}

JSValue api_fetch_promise(JSContext *ctx, JSValueConst this_val, int argc, JSValue *argv) {
    JSValue promise, resolving_funcs[2];
    JSValueConst args[4];

    auto *parent_ctx = (Context *)JS_GetContextOpaque(ctx);

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

//    FetchPromise fetch_promise{};
//    fetch_promise.resolve = JS_DupValue(ctx, resolving_funcs[0]);
//    fetch_promise.reject = JS_DupValue(ctx, resolving_funcs[1]);
//    fetch_promise.request = JS_DupValue(ctx, argv[0]);
//    fetch_promise.options = JS_DupValue(ctx, argv[1]);
//
//    parent_ctx->fetch_mutex.lock();
//    parent_ctx->fetches.push(fetch_promise);
//    parent_ctx->fetch_mutex.unlock();

    return promise;
}

void api_fetch(JSContext *ctx, FetchPromise promise)
{
    JSValue jni_exception;
    JNIEnv *thread_env = nullptr;

    auto *parent_ctx = (Context *)JS_GetContextOpaque(ctx);

    JSValue global_obj = JS_GetGlobalObject(ctx);

    if (JS_IsFunction(ctx, promise.resolve)) {
        auto res = JS_Call(ctx, promise.resolve, global_obj, 0, nullptr);
        if (JS_IsException(res)) {
            jni_exception = res;
        }

        if (!JS_IsNull(res) && !JS_IsUndefined(res)) {
            jni_exception = res;
        }
    }



    JS_FreeValue(ctx, global_obj);

//    auto status = parent_ctx->vm->GetEnv((void**)&thread_env, JNI_VERSION_1_6);
//    if (status == JNI_EDETACHED) {
//        if (parent_ctx->vm->AttachCurrentThread(&thread_env, NULL) != JNI_OK) {
//            // reject
//        }
//    }
//
//    if (JS_IsNull(promise.request)) {
//        // reject
//        return;
//    }
//
//    if (!JS_IsString(promise.request)) {
//        // reject
//        return;
//    }
//
//    auto c_url_str = JS_ToCString(ctx, promise.request);
//
//    auto j_url_str = thread_env->NewStringUTF(c_url_str);
//
//    jclass j_context_api_class = thread_env->FindClass("io/ionic/android_js_engine/ContextAPI");
//    jni_exception = check_and_throw_jni_exception(thread_env, ctx);
//
//    if (JS_IsException(jni_exception)) {
//        // reject
//        return;
//    }
//
//    jmethodID j_method = thread_env->GetMethodID(j_context_api_class, "fetch", "(Ljava/lang/String;)Lio/ionic/android_js_engine/JSResponse;");
//    jni_exception = check_and_throw_jni_exception(thread_env, ctx);
//
//    if (JS_IsException(jni_exception)) {
//        // reject
//        return;
//    }
//
//    auto response = thread_env->CallObjectMethod(parent_ctx->api, j_method, j_url_str);
//    jni_exception = check_and_throw_jni_exception(thread_env, ctx);
//
//    if (JS_IsException(jni_exception)) {
//        // reject
//        JS_Call(ctx, promise.reject, global_obj, 1, (JSValueConst *)&jni_exception);
//        JS_FreeValue(ctx, global_obj);
//        return;
//    }
//
//    JS_Call(ctx, promise.resolve, global_obj, 0, nullptr);
//
//    JS_FreeValue(ctx, global_obj);
}

//JSValue api_fetch(JSContext *ctx, JSValueConst this_val, int argc, JSValue *argv) {
//    JSValue jni_exception, promise;
//
//    if (argc == 0) {
//        return JS_EXCEPTION;
//    }
//
//    if (!JS_IsString(argv[0])) {
//        return JS_EXCEPTION;
//    }
//
//    auto c_url_str = JS_ToCString(ctx, argv[0]);
//
//    auto *parent_ctx = (Context *)JS_GetContextOpaque(ctx);
//
//    auto j_url_str = parent_ctx->env->NewStringUTF(c_url_str);
//
//    jclass j_context_api_class = parent_ctx->env->FindClass("io/ionic/android_js_engine/ContextAPI");
//    jni_exception = check_and_throw_jni_exception(parent_ctx->env, ctx);
//
//    if (JS_IsException(jni_exception)) {
//        return jni_exception;
//    }
//
//    jmethodID j_method = parent_ctx->env->GetMethodID(j_context_api_class, "fetch", "(Ljava/lang/String;)Lio/ionic/android_js_engine/JSResponse;");
//    jni_exception = check_and_throw_jni_exception(parent_ctx->env, ctx);
//
//    if (JS_IsException(jni_exception)) {
//        return jni_exception;
//    }
//
//    JSValue resolving_funcs[2];
//
//    promise = JS_NewPromiseCapability(ctx, resolving_funcs);
//
//    std::string tag = "[Runner Context " + parent_ctx->name + "]";
//
//    auto t = std::thread([resolving_funcs, ctx, tag] {
//        std::this_thread::sleep_for(std::chrono::seconds(1));
//        write_to_logcat(ANDROID_LOG_INFO, tag.c_str(), "calling run request lambda");
//        if (JS_IsFunction(ctx, resolving_funcs[0])) {
//            write_to_logcat(ANDROID_LOG_INFO, tag.c_str(), "is valid function");
//        }
//        JS_Call(ctx, resolving_funcs[0], JS_UNDEFINED, 0, NULL);
//    });
//
//    t.detach();
//
//    JS_FreeCString(ctx, c_url_str);
//
//    FetchPromise fetch_promise{};
//    fetch_promise.promise = JS_DupValue(ctx, promise);
//    fetch_promise.request = JS_DupValue(ctx, argv[0]);
//    fetch_promise.options = JS_DupValue(ctx, argv[1]);
//
//    parent_ctx->fetch_mutex.lock();
//    parent_ctx->fetches.push(fetch_promise);
//    parent_ctx->fetch_mutex.unlock();
//
//    write_to_logcat(ANDROID_LOG_INFO, tag.c_str(), "Returning promise");
//    return promise;
//
//
//
////    JSValue resolve = JS_NewCFunction(ctx, fetch_resolve, "resolve", 1);
////    JSValue reject = JS_NewCFunction(ctx, fetch_reject, "reject", 1);
//
//
//
//
//
////    auto response = parent_ctx->env->CallObjectMethod(parent_ctx->api, j_method, j_url_str);
////    jni_exception = check_and_throw_jni_exception(parent_ctx->env, ctx);
////
////    if (JS_IsException(jni_exception)) {
////        return jni_exception;
////    }
//
//
//
////    return js_response_to_value(ctx, response);
//}
