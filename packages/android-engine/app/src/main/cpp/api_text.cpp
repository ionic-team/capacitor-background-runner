#include "api_text.h"
#include "quickjs/cutils.h"
#include "Context.h"

static JSClassID js_text_encoder_class_id;
static JSClassID js_text_decoder_class_id;

static JSClassDef js_text_encoder_class = {
        "TextEncoder",
};

static JSClassDef js_text_decoder_class = {
        "TextDecoder",
};

static JSValue api_text_encoder_constructor(JSContext *ctx, JSValueConst new_target, int argc, JSValueConst *argv)
{
    JSValue ret_value = JS_UNDEFINED;
    JSValue proto = JS_GetClassProto(ctx, js_text_encoder_class_id);

    ret_value = JS_NewObjectProtoClass(ctx, proto, js_text_encoder_class_id);

    JS_FreeValue(ctx, proto);
    if (JS_IsException(ret_value)) {
        JS_FreeValue(ctx, ret_value);
        return JS_EXCEPTION;
    }

    return ret_value;
}

static JSValue api_text_decoder_constructor(JSContext *ctx, JSValueConst new_target, int argc, JSValueConst *argv)
{
    JSValue ret_value = JS_UNDEFINED;
    JSValue proto = JS_GetClassProto(ctx, js_text_decoder_class_id);

    ret_value = JS_NewObjectProtoClass(ctx, proto, js_text_decoder_class_id);

    JS_FreeValue(ctx, proto);
    if (JS_IsException(ret_value)) {
        JS_FreeValue(ctx, ret_value);
        return JS_EXCEPTION;
    }

    return ret_value;
}

static JSValue api_text_encoder_encode(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    JSValue ret_value = JS_UNDEFINED;
//    size_t elem, len, offset, buf_size;
//
//    if (!JS_IsString(argv[0])) {
//        return JS_EXCEPTION;
//    }
//
//    auto c_str = JS_ToCString(ctx, argv[0]);
//
//    // TODO: Check for JNI Exceptions
//    Context *parent_ctx = (Context *)JS_GetContextOpaque(ctx);
//
//    jclass j_context_class = parent_ctx->env->FindClass("io/ionic/backgroundrunner/android_engine/Context");
//    jmethodID j_method = parent_ctx->env->GetStaticMethodID(j_context_class, "stringToByteArray", "(Ljava/lang/String;)[B");
//
//    jstring j_string = parent_ctx->env->NewStringUTF(c_str);
//
//    jbyteArray byte_array = static_cast<jbyteArray>(parent_ctx->env->CallStaticObjectMethod(j_context_class, j_method, j_string));
//    auto arr = parent_ctx->env->GetByteArrayElements(byte_array, 0);
//
//    size_t length = sizeof(arr) + 1;
//
//    JSValue array_buf = JS_NewArrayBufferCopy(ctx, reinterpret_cast<uint8_t *>(arr), length);
//
//    ret_value = JS_GetTypedArrayBuffer(ctx, array_buf, &offset, &len, &elem);
//
//    parent_ctx->env->ReleaseByteArrayElements(byte_array, arr, 0);
////    parent_ctx->env->ReleaseStringUTFChars(j_string, c_str);
//
//    JS_FreeValue(ctx, array_buf);
//    JS_FreeCString(ctx, c_str);

    return ret_value;
}

static JSValue api_text_encoder_encode_into(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    JSValue ret_value = JS_UNDEFINED;

    return ret_value;
}

static JSValue api_text_decoder_decode(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    JSValue ret_value = JS_UNDEFINED;

    return ret_value;
}

static const JSCFunctionListEntry js_text_encoder_proto_funcs[] = {
    JS_CFUNC_DEF("encode", 1, api_text_encoder_encode ),
    JS_CFUNC_DEF("encodeInto", 2, api_text_encoder_encode_into ),
};

static const JSCFunctionListEntry js_text_decoder_proto_funcs[] = {
    JS_CFUNC_DEF("decode", 1, api_text_decoder_decode ),
};

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


void init_text_encoder_class(JSContext *ctx)
{
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