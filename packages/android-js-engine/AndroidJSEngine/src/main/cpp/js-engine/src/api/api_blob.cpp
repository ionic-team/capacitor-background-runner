#include "api_blob.h"

#include <vector>

#include "../context.hpp"
#include "../errors.hpp"
#include "../quickjs/cutils.h"

struct blob_data {
  uint8_t *data;
  size_t size;
};

static JSClassID js_blob_class_id;

JSValue blob_text_job(JSContext *ctx, int argc, JSValueConst *argv) {
  JSValue resolve, reject, blob_val;

  resolve = argv[0];
  reject = argv[1];
  blob_val = argv[2];

  auto *blob = (blob_data *)JS_GetOpaque(blob_val, js_blob_class_id);
  if (blob == nullptr) {
    auto js_error = create_js_error("backing data is null", ctx);
    reject_promise(ctx, reject, js_error);
    JS_FreeValue(ctx, js_error);
    return JS_UNDEFINED;
  }

  auto *context = (Context *)JS_GetContextOpaque(ctx);
  if (context == nullptr) {
    auto js_error = create_js_error("context is null", ctx);
    reject_promise(ctx, reject, js_error);
    JS_FreeValue(ctx, js_error);
    return JS_UNDEFINED;
  }

  try {
    JSValue text_val;

    if (blob->size == 0) {
      text_val = JS_NewString(ctx, "");
    } else {
      auto text_string = context->native_interface->byte_array_to_str(blob->data, 0, "utf-8").c_str();
      text_val = JS_NewString(ctx, text_string);
    }

    JSValueConst resolve_args[1];
    resolve_args[0] = text_val;

    auto global_obj = JS_GetGlobalObject(ctx);
    JS_Call(ctx, resolve, global_obj, 1, resolve_args);

    JS_FreeValue(ctx, text_val);
    JS_FreeValue(ctx, global_obj);

    return JS_UNDEFINED;
  } catch (std::exception &ex) {
    return JS_UNDEFINED;
  }
}

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
  JSValue promise, resolving_funcs[2];
  JSValueConst args[3];

  promise = JS_NewPromiseCapability(ctx, resolving_funcs);
  if (JS_IsException(promise)) {
    return promise;
  }

  args[0] = resolving_funcs[0];
  args[1] = resolving_funcs[1];
  args[2] = this_val;

  JS_EnqueueJob(ctx, blob_text_job, 4, args);

  JS_FreeValue(ctx, resolving_funcs[0]);
  JS_FreeValue(ctx, resolving_funcs[1]);

  return promise;
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

  std::vector<uint8_t> data_components(0);

  JSValue arr_val;
  uint32_t input_size, i;

  JSValue length_val = JS_GetPropertyStr(ctx, input_arr, "length");
  JS_ToUint32(ctx, &input_size, length_val);

  for (i = 0; i < input_size; i++) {
    JSValue val = JS_GetPropertyUint32(ctx, input_arr, i);
    if (JS_IsString(val)) {
      auto str = JS_ToCString(ctx, val);
      while (*str != '\0') {
        data_components.push_back(static_cast<uint8_t>(*str));
        ++str;
      }

      JS_FreeCString(ctx, str);
      continue;
    }

    if (JS_IsObject(val)) {
      // Handling TypedArray
      size_t obj_size, obj_offset, obj_bytes;
      auto arr = JS_GetTypedArrayBuffer(ctx, val, &obj_offset, &obj_size, &obj_bytes);

      if (!JS_IsException(arr)) {
        context->native_interface->logger(LoggerLevel::DEBUG, "Blob", "value is an array");
        JS_FreeValue(ctx, arr);
        continue;
      }

      // Handling ArrayBuffer
      auto arr_buf = JS_GetArrayBuffer(ctx, &obj_size, val);
      if (arr_buf != nullptr) {
        for (int x = 0; x < obj_size; x++) {
          data_components.push_back(arr_buf[x]);
        }

        JS_FreeValue(ctx, arr);
        continue;
      }
    }
  }

  auto *blob = new blob_data;
  blob->data = data_components.data();
  blob->size = data_components.size();

  JS_SetOpaque(new_object, blob);

  JS_SetPropertyStr(ctx, new_object, "size", JS_NewInt32(ctx, blob->size));

  return new_object;
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
