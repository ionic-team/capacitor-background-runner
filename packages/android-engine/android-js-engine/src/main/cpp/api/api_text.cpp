#include "api_text.h"
#include "../context.h"
#include "../errors.h"
#include "../quickjs/cutils.h"

static JSClassID js_text_encoder_class_id;
static JSClassID js_text_decoder_class_id;

static JSClassDef js_text_encoder_class = {
        "TextEncoder",
};

static JSClassDef js_text_decoder_class = {
        "TextDecoder",
};

static JSValue api_text_encoder_constructor(JSContext *ctx, JSValueConst new_target, int argc, JSValueConst *argv) {
    JSValue const proto = JS_GetClassProto(ctx, js_text_encoder_class_id);

    auto new_object_class = JS_NewObjectProtoClass(ctx, proto, js_text_encoder_class_id);

    JS_FreeValue(ctx, proto);

    return new_object_class;
}

static JSValue api_text_decoder_constructor(JSContext *ctx, JSValueConst new_target, int argc, JSValueConst *argv) {
    JSValue const proto = JS_GetClassProto(ctx, js_text_decoder_class_id);

    auto new_object_class = JS_NewObjectProtoClass(ctx, proto, js_text_decoder_class_id);

    JS_FreeValue(ctx, proto);

    if (JS_IsException(new_object_class)) {
        return new_object_class;
    }

    JS_SetPropertyStr(ctx, new_object_class, "label", JS_NewString(ctx, "utf-8"));

    if (argc == 1) {
        if (JS_IsString(argv[0])) {
            JS_SetPropertyStr(ctx, new_object_class, "label", argv[0]);
        }
    }

    return new_object_class;
}

static JSValue api_text_encoder_encode(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    uint8_t *buf;

    if (!JS_IsString(argv[0])) {
        return JS_EXCEPTION;
    }

    auto *context = (Context *)JS_GetContextOpaque(ctx);

    auto *env = context->java->getEnv();
    if (env == nullptr) {
        return throw_js_exception(ctx, "JVM Environment is null");
    }

    const auto *c_str = JS_ToCString(ctx, argv[0]);
    jstring j_string = env->NewStringUTF(c_str);

    auto *byte_array = static_cast<jbyteArray>(env->CallStaticObjectMethod(context->java->web_api_class, context->java->web_api_stringToByteArray_method, j_string));
    auto exception = throw_jvm_exception_in_js(env, ctx);
    if (JS_IsException(exception)) {
        JS_FreeCString(ctx, c_str);
        return exception;
    }

    auto length = env->GetArrayLength(byte_array);
    auto arr = env->GetByteArrayElements(byte_array, 0);
    buf = new uint8_t(length);

    for (int i = 0; i < length; i++) {
        buf[i] = arr[i];
    }

    auto array_buffer = JS_NewArrayBuffer(ctx, buf, length, nullptr, nullptr, 0);
    auto ret_value = JS_NewUInt8Array(ctx, array_buffer, 0, length);

    env->ReleaseByteArrayElements(byte_array,arr, 0);
    JS_FreeCString(ctx, c_str);
    JS_FreeValue(ctx, array_buffer);

    return ret_value;
}

static JSValue api_text_decoder_decode(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    uint8_t *buf;
    size_t elem, len, offset, buf_size;
    int size;

    auto t_arr_buf = JS_GetTypedArrayBuffer(ctx, argv[0], &offset, &len, &elem);
    buf = JS_GetArrayBuffer(ctx, &buf_size, t_arr_buf);

    if (!buf) {
        JS_FreeValue(ctx, t_arr_buf);
        return JS_EXCEPTION;
    }

    size = buf_size;

    JS_FreeValue(ctx, t_arr_buf);

    auto *context = (Context *)JS_GetContextOpaque(ctx);

    auto *env = context->java->getEnv();
    if (env == nullptr) {
        return throw_js_exception(ctx, "JVM Environment is null");
    }

    auto labelPropStr = JS_GetPropertyStr(ctx, this_val, "label");
    const auto *encoding = JS_ToCString(ctx, labelPropStr);

    auto *j_encoding = env->NewStringUTF(encoding);

    jbyteArray byte_array = env->NewByteArray(size);
    auto exception = throw_jvm_exception_in_js(env, ctx);
    if (JS_IsException(exception)) {
        JS_FreeCString(ctx, encoding);
        return exception;
    }

    env->SetByteArrayRegion(byte_array, 0, size, reinterpret_cast<const jbyte *>(buf));
    exception = throw_jvm_exception_in_js(env, ctx);
    if (JS_IsException(exception)) {
        JS_FreeCString(ctx, encoding);
        return exception;
    }

    auto *str = (jstring)env->CallStaticObjectMethod(context->java->web_api_class, context->java->web_api_byteArrayToString_method, byte_array, j_encoding);
    exception = throw_jvm_exception_in_js(env, ctx);
    if (JS_IsException(exception)) {
        JS_FreeCString(ctx, encoding);
        return exception;
    }

    const char *c_str = env->GetStringUTFChars(str, nullptr);

    auto ret_value = JS_NewString(ctx, c_str);

    env->ReleaseStringUTFChars(str, c_str);
    JS_FreeCString(ctx, encoding);

    return ret_value;
}

static const JSCFunctionListEntry js_text_encoder_proto_funcs[] = {
        JS_CFUNC_DEF("encode", 1, api_text_encoder_encode),
};

static const JSCFunctionListEntry js_text_decoder_proto_funcs[] = {
        JS_CFUNC_DEF("decode", 1, api_text_decoder_decode),
};

void init_text_encoder_class(JSContext *ctx) {
    JSValue text_encoder_proto, obj;

    JS_NewClassID(&js_text_encoder_class_id);
    JS_NewClass(JS_GetRuntime(ctx), js_text_encoder_class_id, &js_text_encoder_class);
    text_encoder_proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, text_encoder_proto, js_text_encoder_proto_funcs, countof(js_text_encoder_proto_funcs));
    JS_SetClassProto(ctx, js_text_encoder_class_id, text_encoder_proto);

    obj = JS_NewCFunction2(ctx, api_text_encoder_constructor, "TextEncoder", 0, JS_CFUNC_constructor, 0);

    JSValue global_obj = JS_GetGlobalObject(ctx);
    JS_SetPropertyStr(ctx, global_obj, "TextEncoder", obj);
    JS_FreeValue(ctx, global_obj);
}

void init_text_decoder_class(JSContext *ctx) {
    JSValue text_decoder_proto, obj;

    JS_NewClassID(&js_text_decoder_class_id);
    JS_NewClass(JS_GetRuntime(ctx), js_text_decoder_class_id, &js_text_decoder_class);
    text_decoder_proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, text_decoder_proto, js_text_decoder_proto_funcs, countof(js_text_decoder_proto_funcs));
    JS_SetClassProto(ctx, js_text_decoder_class_id, text_decoder_proto);

    obj = JS_NewCFunction2(ctx, api_text_decoder_constructor, "TextDecoder", 0, JS_CFUNC_constructor, 0);

    JSValue global_obj = JS_GetGlobalObject(ctx);
    JS_SetPropertyStr(ctx, global_obj, "TextDecoder", obj);
    JS_FreeValue(ctx, global_obj);
}
