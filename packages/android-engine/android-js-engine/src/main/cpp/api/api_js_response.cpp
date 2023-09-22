#include "api_js_response.h"
#include "../quickjs/cutils.h"
#include "../context.h"
#include "../errors.h"

struct response_data {
    uint8_t *data;
    size_t size;
};

static JSClassID js_response_class_id;

JSValue js_response_json_job(JSContext *ctx, int argc, JSValueConst *argv) {
    JSValue resolve, reject, response;

    resolve = argv[0];
    reject = argv[1];
    response = argv[2];

    auto *data = (response_data *)JS_GetOpaque(response, js_response_class_id);
    if (data == nullptr) {
        auto exception = throw_js_exception(ctx, "backing data is null");
        reject_promise(ctx, reject, exception);
        JS_FreeValue(ctx, exception);
        return JS_UNDEFINED;
    }

    auto *context = (Context *)JS_GetContextOpaque(ctx);

    auto *env = context->java->getEnv();
    if (env == nullptr) {
        auto exception = throw_js_exception(ctx, "JVM Environment is null");
        reject_promise(ctx, reject, exception);
        JS_FreeValue(ctx, exception);
        return JS_UNDEFINED;
    }

    jstring encoding = env->NewStringUTF("utf-8");
    jbyteArray j_byte_array = env->NewByteArray(data->size);

    env->SetByteArrayRegion(j_byte_array, 0, data->size, reinterpret_cast<const jbyte *>(data->data));

    auto json_string = (jstring)env->CallStaticObjectMethod(context->java->web_api_class, context->java->web_api_byteArrayToString_method, j_byte_array, encoding);
    auto exception = throw_jvm_exception_in_js(env, ctx);
    if (JS_IsException(exception)) {
        reject_promise(ctx, reject, exception);
        JS_FreeValue(ctx, exception);
        return JS_UNDEFINED;
    }

    const char *json_c_string = env->GetStringUTFChars(json_string, nullptr);

    auto global_obj = JS_GetGlobalObject(ctx);

    if (json_c_string != nullptr && strlen(json_c_string) > 0) {
        auto json = JS_ParseJSON(ctx, json_c_string, strlen(json_c_string), "<json>");
        auto res = JS_Call(ctx, resolve, global_obj, 1, (JSValueConst *)&json);
        JS_FreeValue(ctx, res);
        JS_FreeValue(ctx, json);
    } else {
        auto res = JS_Call(ctx, resolve, global_obj, 0, nullptr);
        JS_FreeValue(ctx, res);
    }

    JS_FreeValue(ctx, global_obj);

    return JS_UNDEFINED;
}

JSValue js_response_text_job(JSContext *ctx, int argc, JSValueConst *argv) {
    JSValue resolve, reject, response;

    resolve = argv[0];
    reject = argv[1];
    response = argv[2];

    auto *data = (response_data *)JS_GetOpaque(response, js_response_class_id);
    if (data == nullptr) {
        auto exception = throw_js_exception(ctx, "backing data is null");
        reject_promise(ctx, reject, exception);
        JS_FreeValue(ctx, exception);
        return JS_UNDEFINED;
    }

    auto *context = (Context *)JS_GetContextOpaque(ctx);

    auto *env = context->java->getEnv();
    if (env == nullptr) {
        auto exception = throw_js_exception(ctx, "JVM Environment is null");
        reject_promise(ctx, reject, exception);
        JS_FreeValue(ctx, exception);
        return JS_UNDEFINED;
    }

    jstring encoding = env->NewStringUTF("utf-8");
    jbyteArray j_byte_array = env->NewByteArray(data->size);

    env->SetByteArrayRegion(j_byte_array, 0, data->size, reinterpret_cast<const jbyte *>(data->data));

    auto text_string = (jstring)env->CallStaticObjectMethod(context->java->web_api_class, context->java->web_api_byteArrayToString_method, j_byte_array, encoding);
    auto exception = throw_jvm_exception_in_js(env, ctx);
    if (JS_IsException(exception)) {
        reject_promise(ctx, reject, exception);
        JS_FreeValue(ctx, exception);
        return JS_UNDEFINED;
    }

    const char *text_c_string = env->GetStringUTFChars(text_string, nullptr);
    auto text = JS_NewString(ctx, text_c_string);

    JSValueConst resolve_args[1];
    resolve_args[0] = text;

    auto global_obj = JS_GetGlobalObject(ctx);
    JS_Call(ctx, resolve, global_obj, 1, resolve_args);

    JS_FreeValue(ctx, text);
    JS_FreeValue(ctx, global_obj);

    return JS_UNDEFINED;
}

JSValue js_response_array_buffer_job(JSContext *ctx, int argc, JSValueConst *argv) {
    JSValue resolve, reject, response;

    resolve = argv[0];
    reject = argv[1];
    response = argv[2];

    auto *data = (response_data *)JS_GetOpaque(response, js_response_class_id);
    if (data == nullptr) {
        auto exception = throw_js_exception(ctx, "backing data is null");
        reject_promise(ctx, reject, exception);
        JS_FreeValue(ctx, exception);
        return JS_UNDEFINED;
    }

    auto array_buffer = JS_NewArrayBuffer(ctx, data->data, data->size, nullptr, nullptr, 0);
    auto buf = JS_NewUInt8Array(ctx, array_buffer, 0, data->size);

    auto global_obj = JS_GetGlobalObject(ctx);
    auto res = JS_Call(ctx, resolve, global_obj, 1, (JSValueConst *)&buf);

    JS_FreeValue(ctx, res);
    JS_FreeValue(ctx, buf);
    JS_FreeValue(ctx, array_buffer);
    JS_FreeValue(ctx, global_obj);

    return JS_UNDEFINED;
}

JSValue js_response_blob_job(JSContext *ctx, int argc, JSValueConst *argv) {
    JSValue resolve, reject, response;

    resolve = argv[0];
    reject = argv[1];
    response = argv[2];

    auto *data = (response_data *)JS_GetOpaque(response, js_response_class_id);
    if (data == nullptr) {
        auto exception = throw_js_exception(ctx, "backing data is null");
        reject_promise(ctx, reject, exception);
        JS_FreeValue(ctx, exception);
        return JS_UNDEFINED;
    }

    auto blob = new_blob(ctx, data->data, data->size);

    auto global_obj = JS_GetGlobalObject(ctx);
    auto res = JS_Call(ctx, resolve, global_obj, 1, (JSValueConst *)&blob);

    JS_FreeValue(ctx, res);
    JS_FreeValue(ctx, global_obj);
    JS_FreeValue(ctx, blob);

    return JS_UNDEFINED;
}

JSValue js_response_json(JSContext *ctx, JSValueConst this_val, int argc, JSValue *argv) {
    JSValue promise, resolving_funcs[2];
    JSValueConst args[3];

    promise = JS_NewPromiseCapability(ctx, resolving_funcs);
    if (JS_IsException(promise)) {
        return promise;
    }

    args[0] = resolving_funcs[0];
    args[1] = resolving_funcs[1];
    args[2] = this_val;

    JS_EnqueueJob(ctx, js_response_json_job, 4, args);

    JS_FreeValue(ctx, resolving_funcs[0]);
    JS_FreeValue(ctx, resolving_funcs[1]);

    return promise;
}

JSValue js_response_text(JSContext *ctx, JSValueConst this_val, int argc, JSValue *argv) {
    JSValue promise, resolving_funcs[2];
    JSValueConst args[3];

    promise = JS_NewPromiseCapability(ctx, resolving_funcs);
    if (JS_IsException(promise)) {
        return promise;
    }

    args[0] = resolving_funcs[0];
    args[1] = resolving_funcs[1];
    args[2] = this_val;

    JS_EnqueueJob(ctx, js_response_text_job, 4, args);

    JS_FreeValue(ctx, resolving_funcs[0]);
    JS_FreeValue(ctx, resolving_funcs[1]);

    return promise;
}

JSValue js_response_blob(JSContext *ctx, JSValueConst this_val, int argc, JSValue *argv) {
    JSValue promise, resolving_funcs[2];
    JSValueConst args[3];

    promise = JS_NewPromiseCapability(ctx, resolving_funcs);
    if (JS_IsException(promise)) {
        return promise;
    }

    args[0] = resolving_funcs[0];
    args[1] = resolving_funcs[1];
    args[2] = this_val;

    JS_EnqueueJob(ctx, js_response_blob_job, 4, args);

    JS_FreeValue(ctx, resolving_funcs[0]);
    JS_FreeValue(ctx, resolving_funcs[1]);

    return promise;
}

JSValue js_response_array_buffer(JSContext *ctx, JSValueConst this_val, int argc, JSValue *argv) {
    JSValue promise, resolving_funcs[2];
    JSValueConst args[3];

    promise = JS_NewPromiseCapability(ctx, resolving_funcs);
    if (JS_IsException(promise)) {
        return promise;
    }

    args[0] = resolving_funcs[0];
    args[1] = resolving_funcs[1];
    args[2] = this_val;

    JS_EnqueueJob(ctx, js_response_array_buffer_job, 4, args);

    JS_FreeValue(ctx, resolving_funcs[0]);
    JS_FreeValue(ctx, resolving_funcs[1]);

    return promise;
}

static void js_response_data_finalizer(JSRuntime *rt, JSValue val) {
    auto *data = (response_data *)JS_GetOpaque(val, js_response_class_id);
    if (data != nullptr) {
        delete data;
    }
}

static JSClassDef js_response_class = {
        "Response",
        .finalizer = js_response_data_finalizer
};

static const JSCFunctionListEntry js_response_proto_funcs[] = {
        JS_CFUNC_DEF("json", 0, js_response_json),
        JS_CFUNC_DEF("text", 0, js_response_text),
        JS_CFUNC_DEF("arrayBuffer", 0, js_response_array_buffer),
        JS_CFUNC_DEF("blob", 0, js_response_blob),
};

static JSValue api_response_constructor(JSContext *ctx, JSValueConst new_target, int argc, JSValueConst *argv) {
    auto proto = JS_GetClassProto(ctx, js_response_class_id);
    auto new_object = JS_NewObjectProtoClass(ctx, proto, js_response_class_id);

    JS_FreeValue(ctx, proto);

    return new_object;
}

JSValue new_js_response(JSContext *ctx, jobject j_response) {
    auto *context = (Context *)JS_GetContextOpaque(ctx);

    auto *env = context->java->getEnv();
    if (env == nullptr) {
        return throw_js_exception(ctx, "JVM Environment is null");
    }

    auto j_ok = env->GetBooleanField(j_response, context->java->js_response_ok_field);
    auto j_status = env->GetIntField(j_response, context->java->js_response_status_field);
    auto *j_url = (jstring)env->GetObjectField(j_response, context->java->js_response_url_field);
    auto *j_data = static_cast<jbyteArray>(env->GetObjectField(j_response, context->java->js_response_data_field));
    auto *j_err = (jstring)env->GetObjectField(j_response, context->java->js_response_error_field);

    const auto *c_url = env->GetStringUTFChars(j_url, nullptr);

    auto response = api_response_constructor(ctx, JS_UNDEFINED, 0, nullptr);

    JS_SetPropertyStr(ctx, response, "ok", JS_NewBool(ctx, (int)j_ok));
    JS_SetPropertyStr(ctx, response, "status", JS_NewInt32(ctx, (int)j_status));
    JS_SetPropertyStr(ctx, response, "url", JS_NewString(ctx, c_url));
    JS_SetPropertyStr(ctx, response, "error", JS_NULL);

    if (j_data != nullptr) {
        auto j_data_len = env->GetArrayLength(j_data);
        auto *j_data_arr = env->GetByteArrayElements(j_data, 0);

        auto *buf = new uint8_t(j_data_len);

        for (int i = 0; i < j_data_len; i++) {
            buf[i] = j_data_arr[i];
        }

        env->ReleaseByteArrayElements(j_data, j_data_arr, 0);

        // attach the array buffer to the js_response
        auto * res_data = new response_data;
        res_data->data = buf;
        res_data->size = j_data_len;

        JS_SetOpaque(response, res_data);
    }

    if (j_err != nullptr) {
        const auto *c_err = env->GetStringUTFChars(j_err, nullptr);
        JS_SetPropertyStr(ctx, response, "error", JS_NewString(ctx, c_err));
        env->ReleaseStringUTFChars(j_err, c_err);
    }

    env->ReleaseStringUTFChars(j_url, c_url);

    return response;
}

void init_response_class(JSContext *ctx) {
    JSValue response_proto, obj;

    JS_NewClassID(&js_response_class_id);
    JS_NewClass(JS_GetRuntime(ctx), js_response_class_id, &js_response_class);

    response_proto = JS_NewObject(ctx);

    JS_SetPropertyFunctionList(ctx, response_proto, js_response_proto_funcs, countof(js_response_proto_funcs));
    JS_SetClassProto(ctx, js_response_class_id, response_proto);

    obj = JS_NewCFunction2(ctx, api_response_constructor, "Response", 1, JS_CFUNC_constructor, 1);

    auto global_obj = JS_GetGlobalObject(ctx);
    JS_SetPropertyStr(ctx, global_obj, "Response", obj);
    JS_FreeValue(ctx, global_obj);
}


