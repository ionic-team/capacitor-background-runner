#include "api_filereader.h"

#include "../context.hpp"
#include "../errors.hpp"
#include "../quickjs/cutils.h"
#include "api_blob.h"

FileReaderStore::FileReaderStore(JSContext *ctx) { this->ctx = ctx; }
FileReaderStore::~FileReaderStore() {
  for (const auto &kv : this->event_listeners) {
    JS_FreeValue(this->ctx, kv.second);
  }
  this->event_listeners.clear();
}

static JSClassID js_file_reader_class_id;

static void js_file_reader_data_finalizer(JSRuntime *rt, JSValue val) {
  auto *file_reader = (FileReaderStore *)JS_GetOpaque(val, js_file_reader_class_id);
  if (file_reader != nullptr) {
    delete file_reader;
  }
}

static JSClassDef js_file_reader_class = {"FileReader", .finalizer = js_file_reader_data_finalizer};

static JSValue api_file_reader_abort(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) { return JS_UNDEFINED; }

static JSValue api_file_reader_read_array_buffer(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) { return JS_UNDEFINED; }

static JSValue api_file_reader_read_data_url(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) { return JS_UNDEFINED; }

static JSValue api_file_reader_read_text(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  auto input = argv[0];

  if (!JS_IsObject(input)) {
    // fail
  }

  auto *blob = (BlobBackingStore *)JS_GetOpaque(input, js_blob_class_id);
  if (blob == nullptr) {
    return create_js_error("backing data is null", ctx);
  }

  auto text = blob->string();

  return JS_NewString(ctx, text.c_str());
}

static JSValue api_file_reader_add_event_listener(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  auto *file_reader = (FileReaderStore *)JS_GetOpaque(this_val, js_file_reader_class_id);
  if (file_reader == nullptr) {
    return create_js_error("file reader store is null", ctx);
  }

  const char *event = JS_ToCString(ctx, argv[0]);
  JSValue callback = argv[1];

  if (!JS_IsFunction(ctx, callback)) {
  }

  callback = JS_DupValue(ctx, callback);

  auto itr = file_reader->event_listeners.find(event);

  if (itr == file_reader->event_listeners.end()) {
    file_reader->event_listeners.emplace(event, callback);
  } else {
    JS_FreeValue(ctx, itr->second);
    itr->second = callback;
  }

  JS_FreeCString(ctx, event);

  return JS_UNDEFINED;
}

static JSValue api_file_reader_remove_event_listener(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) { return JS_UNDEFINED; }

static const JSCFunctionListEntry js_file_reader_proto_funcs[] = {
    JS_CFUNC_DEF("abort", 0, api_file_reader_abort),
    JS_CFUNC_DEF("readAsArrayBuffer", 1, api_file_reader_read_array_buffer),
    JS_CFUNC_DEF("readAsDataURL", 1, api_file_reader_read_data_url),
    JS_CFUNC_DEF("readAsText", 2, api_file_reader_read_text),
    JS_CFUNC_DEF("addEventListener", 2, api_file_reader_add_event_listener),
    JS_CFUNC_DEF("removeEventListener", 2, api_file_reader_remove_event_listener),
};

static JSValue api_file_reader_constructor(JSContext *ctx, JSValueConst new_target, int argc, JSValueConst *argv) {
  auto proto = JS_GetClassProto(ctx, js_file_reader_class_id);
  auto new_object = JS_NewObjectProtoClass(ctx, proto, js_file_reader_class_id);

  JS_FreeValue(ctx, proto);

  auto file_reader = new FileReaderStore(ctx);

  JS_SetOpaque(new_object, file_reader);

  JS_SetPropertyStr(ctx, new_object, "readyState", JS_NewInt32(ctx, 0));
  JS_SetPropertyStr(ctx, new_object, "result", JS_NULL);
  JS_SetPropertyStr(ctx, new_object, "error", JS_NULL);

  return new_object;
}

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