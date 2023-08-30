#include "api_blob.h"
#include "quickjs/cutils.h"

struct blob_data {
    uint8_t *data;
    size_t size;
    JSContext *ctx;
};

static JSClassID js_blob_class_id;

static void js_blob_data_finalizer(JSRuntime *rt, JSValue val) {
    auto *blob = (blob_data*)JS_GetOpaque(val, js_blob_class_id);
    if (blob != nullptr) {
        delete blob;
    }
}

static JSClassDef js_blob_class = {
        "Blob",
        .finalizer = js_blob_data_finalizer
};

static JSValue api_blob_get_array_buffer(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {

}

static JSValue api_blob_get_text(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    auto *blob = (blob_data*)JS_GetOpaque(this_val, js_blob_class_id);
    if (blob == nullptr) {
        return JS_UNDEFINED;
    }


}

static const JSCFunctionListEntry js_blob_proto_funcs[] = {
        JS_CFUNC_DEF("arrayBuffer", 0, api_blob_get_array_buffer),
        JS_CFUNC_DEF("text", 0, api_blob_get_text),
};

static JSValue api_blob_constructor(JSContext *ctx, JSValueConst new_target, int argc, JSValueConst *argv) {
    JSValue ret_value = JS_UNDEFINED;
    auto proto = JS_GetClassProto(ctx, js_blob_class_id);

    ret_value = JS_NewObjectProtoClass(ctx, proto, js_blob_class_id);

    JSValue input_arr = argv[0];

    if (!JS_IsArray(ctx, input_arr)) {
        input_arr = JS_NewArray(ctx);
    }

    JS_GetA

    uint8_t *data;
    size_t size, offset, bytes;

    auto typed_arr = JS_GetTypedArrayBuffer(ctx, input_arr , &offset, &size, &bytes);

    data = JS_GetArrayBuffer(ctx, &size, typed_arr);

    auto * blob = new blob_data;
    blob->data = data;
    blob->size = size;
    blob->ctx = ctx;

    JS_SetOpaque(ret_value, blob);

    JS_FreeValue(ctx, proto);

    return ret_value;
}

void init_blob_class(JSContext *ctx) {
    JSValue blob_proto, obj;

    JS_NewClassID(&js_blob_class_id);
    JS_NewClass(JS_GetRuntime(ctx), js_blob_class_id, &js_blob_class);

    blob_proto = JS_NewObject(ctx);

    JS_SetPropertyFunctionList(ctx, blob_proto, js_blob_proto_funcs, countof(js_blob_proto_funcs));
    JS_SetClassProto(ctx, js_blob_class_id, blob_proto);

    obj = JS_NewCFunction2(ctx, api_blob_constructor, "Blob", 1, JS_CFUNC_constructor, 1);

    JSValue global_obj = JS_GetGlobalObject(ctx);
    JS_SetPropertyStr(ctx, global_obj, "Blob", obj);
    JS_FreeValue(ctx, global_obj);
}
