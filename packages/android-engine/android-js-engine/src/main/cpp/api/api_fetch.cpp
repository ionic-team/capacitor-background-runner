#include "api_fetch.h"
#include "../context.h"
#include "../errors.h"

JSValue js_fetch_job(JSContext *ctx, int argc, JSValueConst *argv) {
    JSValue resolve, reject, request, options;

    resolve = argv[0];
    reject = argv[1];
    request = argv[2];
    options = argv[3];

    auto *context = (Context *)JS_GetContextOpaque(ctx);

    auto *env = context->java->getEnv();
    if (env == nullptr) {
        auto exception = throw_js_exception(ctx, "JVM Environment is null");
        reject_promise(ctx, reject, exception);
        JS_FreeValue(ctx, exception);
        return JS_UNDEFINED;
    }

    if (JS_IsNull(request) || !JS_IsString(request)) {
        auto exception = throw_js_exception(ctx, "invalid url");
        reject_promise(ctx, reject, exception);
        JS_FreeValue(ctx, exception);
        return JS_UNDEFINED;
    }

    const auto *c_url_str = JS_ToCString(ctx, request);
    auto *j_url_str = env->NewStringUTF(c_url_str);

    jobject j_options = nullptr;

    if (!JS_IsNull(options) && !JS_IsUndefined(options)) {
        auto options_json_value = JS_JSONStringify(ctx, options, JS_UNDEFINED, JS_UNDEFINED);
        const auto *options_json_c_str = JS_ToCString(ctx, options_json_value);
        auto *options_json_j_str = env->NewStringUTF(options_json_c_str);

        j_options = env->NewObject(context->java->js_fetch_options_class, context->java->js_fetch_options_constructor, options_json_j_str);
        auto exception = throw_jvm_exception_in_js(env, ctx);
        if (JS_IsException(exception)) {
            reject_promise(ctx, reject, exception);
            JS_FreeValue(ctx, exception);
            return JS_UNDEFINED;
        }

        JS_FreeCString(ctx, options_json_c_str);
        JS_FreeValue(ctx, options_json_value);
    }

    auto response = env->CallStaticObjectMethod(context->java->web_api_class, context->java->web_api_fetch_method, j_url_str, j_options);
    auto exception = throw_jvm_exception_in_js(env, ctx);
    if (JS_IsException(exception)) {
        reject_promise(ctx, reject, exception);
        JS_FreeValue(ctx, exception);
        return JS_UNDEFINED;
    }

    auto js_response = new_js_response(ctx, response);

    auto ok = JS_GetPropertyStr(ctx, js_response, "ok");
    if (!JS_ToBool(ctx, ok)) {
        auto err = JS_GetPropertyStr(ctx, js_response, "error");
        reject_promise(ctx, reject, err);

        JS_FreeValue(ctx, ok);
        JS_FreeValue(ctx, err);
        JS_FreeValue(ctx, js_response);
        return JS_UNDEFINED;
    }

    auto global_obj = JS_GetGlobalObject(ctx);
    JS_Call(ctx, resolve, global_obj, 1, (JSValueConst *) &js_response);

    JS_FreeValue(ctx, ok);
    JS_FreeValue(ctx, global_obj);
    JS_FreeValue(ctx, js_response);

    return JS_UNDEFINED;
}

JSValue api_fetch(JSContext *ctx, JSValueConst this_val, int argc, JSValue *argv) {
    JSValue  promise, resolving_funcs[2];
    JSValueConst args[4];

    promise = JS_NewPromiseCapability(ctx, resolving_funcs);
    if (JS_IsException(promise)) {
        return promise;
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
