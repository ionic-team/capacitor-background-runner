#include "api_filereader.h"

#include "../context.hpp"
#include "../errors.hpp"
#include "../quickjs/cutils.h"

static JSClassID js_file_reader_class_id;

static void js_file_reader_data_finalizer(JSRuntime *rt, JSValue val) {}

static JSClassDef js_file_reader_class = {"FileReader", .finalizer = js_file_reader_data_finalizer};

static JSValue api_file_reader_abort(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {}

static JSValue api_file_reader_read_array_buffer(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {}

static JSValue api_file_reader_read_data_url(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {}

static JSValue api_file_reader_read_text(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {}

static JSValue api_file_reader_add_event_listener(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {}

static JSValue api_file_reader_remove_event_listener(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {}

static const JSCFunctionListEntry js_file_reader_proto_funcs[] = {
    JS_CFUNC_DEF("abort", 0, api_file_reader_abort),
    JS_CFUNC_DEF("readAsArrayBuffer", 1, api_file_reader_read_array_buffer),
    JS_CFUNC_DEF("readAsDataURL", 1, api_file_reader_abort),
    JS_CFUNC_DEF("readAsText", 2, api_file_reader_abort),
    JS_CFUNC_DEF("addEventListener", 2, api_file_reader_abort),
    JS_CFUNC_DEF("removeEventListener", 2, api_file_reader_abort),
};

static JSValue api_file_reader_constructor(JSContext *ctx, JSValueConst new_target, int argc, JSValueConst *argv) {}

void init_file_reader_class(JSContext *ctx) {
  JSValue file_reader_proto, obj;

  JS_NewClassID(&js_file_reader_class_id);
  JS_NewClass(JS_GetRuntime(ctx), js_file_reader_class_id, &js_file_reader_class);

  file_reader_proto = JS_NewObject(ctx);

  JS_SetPropertyFunctionList(ctx, file_reader_proto, js_file_reader_proto_funcs, countof(js_file_reader_proto_funcs));
  JS_SetClassProto(ctx, js_file_reader_class_id, file_reader_proto);

  obj = JS_NewCFunction2(ctx, api_file_reader_constructor, "FileReader", 0, JS_CFUNC_constructor, 2);

  JSValue global_obj = JS_GetGlobalObject(ctx);
  JS_SetPropertyStr(ctx, global_obj, "FileReader", obj);
  JS_FreeValue(ctx, global_obj);
}