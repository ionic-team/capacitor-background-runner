#include "api_headers.h"

#include "../context.hpp"
#include "../errors.hpp"
#include "../quickjs/cutils.h"

void HeaderBackingStore::set(std::string name, std::string value) { this->storage[name] = value; }
std::string HeaderBackingStore::get(std::string name) {
  if (!this->storage.contains(name)) {
    throw std::runtime_error("value is not set");
  }

  return this->storage[name];
}
static JSClassID js_headers_class_id;

static void js_headers_data_finalizer(JSRuntime *rt, JSValue val) {
  auto *store = (HeaderBackingStore *)JS_GetOpaque(val, js_headers_class_id);
  if (store != nullptr) {
    delete store;
  }
}

static JSClassDef js_headers_class = {"Headers", .finalizer = js_headers_data_finalizer};

static JSValue api_headers_get(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  JSValue name;

  name = argv[0];

  if (!JS_IsString(name)) {
    return JS_ThrowTypeError(ctx, "name must be a string");
  }

  auto *store = (HeaderBackingStore *)JS_GetOpaque(this_val, js_headers_class_id);
  if (store == nullptr) {
    return create_js_error("backing store is null", ctx);
  }

  auto name_c_str = JS_ToCString(ctx, name);

  try {
    auto value_str = store->get(name_c_str);

    JS_FreeCString(ctx, name_c_str);

    return JS_NewString(ctx, value_str.c_str());
  } catch (std::exception &ex) {
    JS_FreeCString(ctx, name_c_str);
    return JS_NULL;
  }
}

static JSValue api_headers_set(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  JSValue name, value;

  name = argv[0];
  value = argv[1];

  if (!JS_IsString(name)) {
    return JS_ThrowTypeError(ctx, "name must be a string");
  }

  if (!JS_IsString(value)) {
    return JS_ThrowTypeError(ctx, "value must be a string");
  }

  // TODO: Validate name is HTTP Header

  auto *store = (HeaderBackingStore *)JS_GetOpaque(this_val, js_headers_class_id);
  if (store == nullptr) {
    return create_js_error("backing store is null", ctx);
  }

  auto name_c_str = JS_ToCString(ctx, name);
  auto value_c_str = JS_ToCString(ctx, value);

  store->set(name_c_str, value_c_str);

  return JS_UNDEFINED;
}

static const JSCFunctionListEntry js_headers_proto_funcs[] = {
    JS_CFUNC_DEF("get", 1, api_headers_get),
    JS_CFUNC_DEF("set", 2, api_headers_set),
};

static JSValue api_headers_constructor(JSContext *ctx, JSValueConst new_target, int argc, JSValueConst *argv) {
  auto *context = (Context *)JS_GetContextOpaque(ctx);
  if (context == nullptr) {
    auto js_error = create_js_error("context is null", ctx);
    return JS_Throw(ctx, js_error);
  }

  auto proto = JS_GetClassProto(ctx, js_headers_class_id);
  auto new_object = JS_NewObjectProtoClass(ctx, proto, js_headers_class_id);

  JS_FreeValue(ctx, proto);

  auto *store = new HeaderBackingStore();
  JS_SetOpaque(new_object, store);

  return new_object;
}

void init_headers_class(JSContext *ctx) {
  JSValue headers_proto, obj;

  JS_NewClassID(&js_headers_class_id);
  JS_NewClass(JS_GetRuntime(ctx), js_headers_class_id, &js_headers_class);

  headers_proto = JS_NewObject(ctx);

  JS_SetPropertyFunctionList(ctx, headers_proto, js_headers_proto_funcs, countof(js_headers_proto_funcs));
  JS_SetClassProto(ctx, js_headers_class_id, headers_proto);

  obj = JS_NewCFunction2(ctx, api_headers_constructor, "Headers", 1, JS_CFUNC_constructor, 2);

  JSValue global_obj = JS_GetGlobalObject(ctx);
  JS_SetPropertyStr(ctx, global_obj, "Headers", obj);
  JS_FreeValue(ctx, global_obj);
}