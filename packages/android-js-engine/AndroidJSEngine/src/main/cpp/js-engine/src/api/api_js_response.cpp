#include "api_js_response.h"

#include "../context.hpp"
#include "../errors.hpp"
#include "../quickjs/cutils.h"

struct response_data {
  std::vector<uint8_t> data;
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
    auto js_error = create_js_error("backing data is null", ctx);
    reject_promise(ctx, reject, js_error);
    JS_FreeValue(ctx, js_error);
    return JS_UNDEFINED;
  }

  auto *context = (Context *)JS_GetContextOpaque(ctx);
  if (context == nullptr) {
    auto js_error = create_js_error("context is null", ctx);
    return JS_Throw(ctx, js_error);
  }

  try {
    auto json_string = context->native_interface->byte_array_to_str(data->data.data(), data->data.size(), "utf-8");

    auto global_obj = JS_GetGlobalObject(ctx);

    if (!json_string.empty()) {
      auto json = JS_ParseJSON(ctx, json_string.c_str(), strlen(json_string.c_str()), "<json>");
      auto res = JS_Call(ctx, resolve, global_obj, 1, (JSValueConst *)&json);
      JS_FreeValue(ctx, res);
      JS_FreeValue(ctx, json);
    } else {
      auto res = JS_Call(ctx, resolve, global_obj, 0, nullptr);
      JS_FreeValue(ctx, res);
    }

    JS_FreeValue(ctx, global_obj);

    return JS_UNDEFINED;
  } catch (std::exception &ex) {
    auto js_error = create_js_error(ex.what(), ctx);
    reject_promise(ctx, reject, js_error);
    JS_FreeValue(ctx, js_error);
    return JS_UNDEFINED;
  }
}

JSValue js_response_text_job(JSContext *ctx, int argc, JSValueConst *argv) {
  JSValue resolve, reject, response;

  resolve = argv[0];
  reject = argv[1];
  response = argv[2];

  auto *data = (response_data *)JS_GetOpaque(response, js_response_class_id);
  if (data == nullptr) {
    auto js_error = create_js_error("backing data is null", ctx);
    reject_promise(ctx, reject, js_error);
    JS_FreeValue(ctx, js_error);
    return JS_UNDEFINED;
  }

  auto *context = (Context *)JS_GetContextOpaque(ctx);

  try {
    auto text_string = context->native_interface->byte_array_to_str(data->data.data(), data->data.size(), "utf-8");

    auto text = JS_NewString(ctx, text_string.c_str());

    JSValueConst resolve_args[1];
    resolve_args[0] = text;

    auto global_obj = JS_GetGlobalObject(ctx);
    JS_Call(ctx, resolve, global_obj, 1, resolve_args);

    JS_FreeValue(ctx, text);
    JS_FreeValue(ctx, global_obj);

    return JS_UNDEFINED;
  } catch (std::exception &ex) {
    auto js_error = create_js_error(ex.what(), ctx);
    reject_promise(ctx, reject, js_error);
    JS_FreeValue(ctx, js_error);
    return JS_UNDEFINED;
  }
}

JSValue js_response_array_buffer_job(JSContext *ctx, int argc, JSValueConst *argv) {
  JSValue resolve, reject, response;

  resolve = argv[0];
  reject = argv[1];
  response = argv[2];

  auto *data = (response_data *)JS_GetOpaque(response, js_response_class_id);
  if (data == nullptr) {
    auto js_error = create_js_error("backing data is null", ctx);
    reject_promise(ctx, reject, js_error);
    JS_FreeValue(ctx, js_error);
    return JS_UNDEFINED;
  }

  auto array_buffer = JS_NewArrayBuffer(ctx, data->data.data(), data->size, nullptr, nullptr, 0);
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
    auto js_error = create_js_error("backing data is null", ctx);
    reject_promise(ctx, reject, js_error);
    JS_FreeValue(ctx, js_error);
    return JS_UNDEFINED;
  }

  auto blob = new_blob(ctx, data->data.data(), data->size);

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

static JSClassDef js_response_class = {"Response", .finalizer = js_response_data_finalizer};

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

JSValue new_js_response(JSContext *ctx, NativeResponse native_response) {
  // auto *context = (Context *)JS_GetContextOpaque(ctx);

  // auto *j_data = static_cast<jbyteArray>(env->GetObjectField(j_response, context->java->js_response_data_field));

  auto response = api_response_constructor(ctx, JS_UNDEFINED, 0, nullptr);

  JS_DefinePropertyValueStr(ctx, response, "ok", JS_NewBool(ctx, native_response.ok), JS_PROP_C_W_E);
  JS_DefinePropertyValueStr(ctx, response, "status", JS_NewInt32(ctx, native_response.status), JS_PROP_C_W_E);
  JS_DefinePropertyValueStr(ctx, response, "url", JS_NewString(ctx, native_response.url.c_str()), JS_PROP_C_W_E);
  JS_DefinePropertyValueStr(ctx, response, "error", JS_NULL, JS_PROP_C_W_E);

  if (!native_response.data.empty()) {
    auto data = native_response.data.data();

    auto *res_data = new response_data;
    res_data->data = native_response.data;
    res_data->size = native_response.data.size();

    JS_SetOpaque(response, res_data);
  }

  if (!native_response.error.empty()) {
    JS_DefinePropertyValueStr(ctx, response, "error", JS_NewString(ctx, native_response.error.c_str()), JS_PROP_C_W_E);
  }

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
