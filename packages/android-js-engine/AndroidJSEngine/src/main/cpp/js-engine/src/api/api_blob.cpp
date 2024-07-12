#include "api_blob.h"

#include "../context.hpp"
#include "../errors.hpp"
#include "../quickjs/cutils.h"

struct blob_data {
  uint8_t *data;
  size_t size;
};

static JSClassID js_blob_class_id;

static void js_blob_data_finalizer(JSRuntime *rt, JSValue val) {
  auto *blob = (blob_data *)JS_GetOpaque(val, js_blob_class_id);
  if (blob != nullptr) {
    delete blob;
  }
}

static JSClassDef js_blob_class = {"Blob", .finalizer = js_blob_data_finalizer};

static JSValue api_blob_get_array_buffer(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  auto *blob = (blob_data *)JS_GetOpaque(this_val, js_blob_class_id);
  if (blob == nullptr) {
    auto js_error = create_js_error("backing data is null", ctx);
    return JS_Throw(ctx, js_error);
  }

  auto array_buffer = JS_NewArrayBuffer(ctx, blob->data, blob->size, nullptr, nullptr, 0);
  auto buf = JS_NewUInt8Array(ctx, array_buffer, 0, blob->size);

  JS_FreeValue(ctx, array_buffer);

  return buf;
}

static JSValue api_blob_get_text(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  auto *blob = (blob_data *)JS_GetOpaque(this_val, js_blob_class_id);
  if (blob == nullptr) {
    auto js_error = create_js_error("backing data is null", ctx);
    return JS_Throw(ctx, js_error);
  }

  auto *context = (Context *)JS_GetContextOpaque(ctx);
  if (context == nullptr) {
    auto js_error = create_js_error("context is null", ctx);
    return JS_Throw(ctx, js_error);
  }

  // TODO handle exception
  auto text_string = context->native_interface->byte_array_to_str(blob->data, 0, "utf-8").c_str();
  return JS_NewString(ctx, text_string);
}

static const JSCFunctionListEntry js_blob_proto_funcs[] = {
    JS_CFUNC_DEF("arrayBuffer", 0, api_blob_get_array_buffer),
    JS_CFUNC_DEF("text", 0, api_blob_get_text),
};

static JSValue api_blob_constructor(JSContext *ctx, JSValueConst new_target, int argc, JSValueConst *argv) {
  auto *context = (Context *)JS_GetContextOpaque(ctx);
  if (context == nullptr) {
    auto js_error = create_js_error("context is null", ctx);
    return JS_Throw(ctx, js_error);
  }

  auto proto = JS_GetClassProto(ctx, js_blob_class_id);
  auto new_object = JS_NewObjectProtoClass(ctx, proto, js_blob_class_id);

  JS_FreeValue(ctx, proto);

  JSValue input_arr = argv[0];

  if (!JS_IsArray(ctx, input_arr)) {
    JS_FreeValue(ctx, input_arr);
    return create_js_error("Value is not a sequence", ctx);
  }

  JSValue arr_val;
  uint32_t input_size, i;

  JSValue length_val = JS_GetPropertyStr(ctx, input_arr, "length");
  JS_ToUint32(ctx, &input_size, length_val);

  for (i = 0; i < input_size; i++) {
    JSValue val = JS_GetPropertyUint32(ctx, input_arr, i);
    if (JS_IsString(val)) {
      context->native_interface->logger(LoggerLevel::DEBUG, "Blob", "value is a string");
    }
  }

  uint8_t *data;
  size_t size, offset, bytes;

  auto typed_arr = JS_GetTypedArrayBuffer(ctx, input_arr, &offset, &size, &bytes);
  if (JS_IsException(typed_arr)) {
    auto exception = JS_DupValue(ctx, typed_arr);
    JS_FreeValue(ctx, typed_arr);

    return exception;
  }

  // JS_FreeValue(ctx, typed_arr);
  // JS_FreeValue(ctx, input_arr);

  return JS_NULL;

  // data = JS_GetArrayBuffer(ctx, &size, typed_arr);

  // auto *blob = new blob_data;
  // blob->data = data;
  // blob->size = size;

  // JS_SetOpaque(new_object, blob);

  // JS_SetPropertyStr(ctx, new_object, "size", JS_NewInt32(ctx, blob->size));

  // JS_FreeValue(ctx, typed_arr);
  // JS_FreeValue(ctx, input_arr);

  // return new_object;
}

JSValue new_blob(JSContext *ctx, uint8_t *data, size_t size) {
  JSValueConst args[1];

  auto array_buffer = JS_NewArrayBuffer(ctx, data, size, nullptr, nullptr, 0);
  args[0] = JS_NewUInt8Array(ctx, array_buffer, 0, size);

  JS_FreeValue(ctx, array_buffer);

  return api_blob_constructor(ctx, JS_UNDEFINED, 1, args);
}

void init_blob_class(JSContext *ctx) {
  JSValue blob_proto, obj;

  JS_NewClassID(&js_blob_class_id);
  JS_NewClass(JS_GetRuntime(ctx), js_blob_class_id, &js_blob_class);

  blob_proto = JS_NewObject(ctx);

  JS_SetPropertyFunctionList(ctx, blob_proto, js_blob_proto_funcs, countof(js_blob_proto_funcs));
  JS_SetClassProto(ctx, js_blob_class_id, blob_proto);

  obj = JS_NewCFunction2(ctx, api_blob_constructor, "Blob", 2, JS_CFUNC_constructor, 2);

  JSValue global_obj = JS_GetGlobalObject(ctx);
  JS_SetPropertyStr(ctx, global_obj, "Blob", obj);
  JS_FreeValue(ctx, global_obj);
}
