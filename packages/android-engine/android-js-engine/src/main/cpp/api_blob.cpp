#include "api_blob.h"
#include "quickjs/cutils.h"

struct blob_data {
    JSValue arr;
};

static JSClassID js_blob_class_id;

static JSClassDef js_blob_class = {
        "Blob"
};

static JSValue api_blob_get_array_buffer(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {

}

static JSValue api_blob_get_text(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {


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

    auto * blob = new blob_data;
    blob->arr = input_arr;

    JS_SetOpaque(ret_value, blob);

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
