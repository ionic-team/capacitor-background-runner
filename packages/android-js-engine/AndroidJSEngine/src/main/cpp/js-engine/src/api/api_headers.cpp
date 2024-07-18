#include "api_headers.h"

#include "../context.hpp"
#include "../errors.hpp"
#include "../quickjs/cutils.h"

void HeaderBackingStore::set(std::string name, std::string value) { this->storage[name] = value; }
void HeaderBackingStore::append(std::string name, std::string value) {
  auto current_value = this->get(name);
  auto new_value = current_value += ", " + value;

  this->set(name, new_value);
}

std::string HeaderBackingStore::get(std::string name) {
  if (!this->storage.contains(name)) {
    throw std::runtime_error("value is not set");
  }

  return this->storage[name];
}

void HeaderBackingStore::remove(std::string name) {
  if (!this->storage.contains(name)) {
    return;
  }

  this->storage.erase(name);
}

bool HeaderBackingStore::exists(std::string name) { return this->storage.contains(name); }

std::vector<std::string> HeaderBackingStore::keys() {
  std::vector<std::string> keys;

  for (auto kv : this->storage) {
    keys.push_back(kv.first);
  }

  return keys;
}

std::vector<std::string> HeaderBackingStore::values() {
  std::vector<std::string> values;

  for (auto kv : this->storage) {
    values.push_back(kv.second);
  }

  return values;
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

  JS_FreeCString(ctx, name_c_str);
  JS_FreeCString(ctx, value_c_str);

  return JS_UNDEFINED;
}

static JSValue api_headers_append(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  JSValue name, value;

  name = argv[0];
  value = argv[1];

  if (!JS_IsString(name)) {
    return JS_ThrowTypeError(ctx, "name must be a string");
  }

  if (!JS_IsString(value)) {
    return JS_ThrowTypeError(ctx, "value must be a string");
  }

  auto *store = (HeaderBackingStore *)JS_GetOpaque(this_val, js_headers_class_id);
  if (store == nullptr) {
    return create_js_error("backing store is null", ctx);
  }

  auto name_c_str = JS_ToCString(ctx, name);
  auto value_c_str = JS_ToCString(ctx, value);

  store->append(name_c_str, value_c_str);

  JS_FreeCString(ctx, name_c_str);
  JS_FreeCString(ctx, value_c_str);

  return JS_UNDEFINED;
}

static JSValue api_headers_delete(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
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

  store->remove(name_c_str);

  JS_FreeCString(ctx, name_c_str);

  return JS_UNDEFINED;
}

static JSValue api_headers_has(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
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

  bool exists = store->exists(name_c_str);

  JS_FreeCString(ctx, name_c_str);

  return JS_NewBool(ctx, exists);
}

static JSValue api_headers_keys(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  auto *store = (HeaderBackingStore *)JS_GetOpaque(this_val, js_headers_class_id);
  if (store == nullptr) {
    return create_js_error("backing store is null", ctx);
  }

  auto keys = store->keys();

  JSValue arr = JS_NewArray(ctx);
  JSValue push_func = JS_GetPropertyStr(ctx, arr, "push");

  for (auto k : keys) {
    JSValueConst push_args[1];
    JSValue str = JS_NewString(ctx, k.c_str());

    push_args[0] = str;
    JS_Call(ctx, push_func, arr, 1, push_args);

    JS_FreeValue(ctx, str);
  }

  JS_FreeValue(ctx, push_func);

  return arr;
}

static JSValue api_headers_values(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  auto *store = (HeaderBackingStore *)JS_GetOpaque(this_val, js_headers_class_id);
  if (store == nullptr) {
    return create_js_error("backing store is null", ctx);
  }

  auto values = store->values();

  JSValue arr = JS_NewArray(ctx);
  JSValue push_func = JS_GetPropertyStr(ctx, arr, "push");

  for (auto v : values) {
    JSValueConst push_args[1];
    JSValue str = JS_NewString(ctx, v.c_str());

    push_args[0] = str;
    JS_Call(ctx, push_func, arr, 1, push_args);

    JS_FreeValue(ctx, str);
  }

  JS_FreeValue(ctx, push_func);

  return arr;
}

static JSValue api_headers_entries(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  auto *store = (HeaderBackingStore *)JS_GetOpaque(this_val, js_headers_class_id);
  if (store == nullptr) {
    return create_js_error("backing store is null", ctx);
  }

  auto keys = store->keys();

  JSValue arr = JS_NewArray(ctx);
  JSValue push_func = JS_GetPropertyStr(ctx, arr, "push");

  for (auto k : keys) {
    auto value = store->get(k);

    JSValue pair = JS_NewArray(ctx);
    JSValue pair_push_func = JS_GetPropertyStr(ctx, pair, "push");

    JSValue key_string_val = JS_NewString(ctx, k.c_str());
    JSValue value_string_val = JS_NewString(ctx, value.c_str());

    JSValueConst pair_push_args[2];
    pair_push_args[0] = key_string_val;
    pair_push_args[1] = value_string_val;

    JS_Call(ctx, pair_push_func, pair, 2, pair_push_args);

    JSValueConst push_args[1];
    push_args[0] = pair;
    JS_Call(ctx, push_func, arr, 1, push_args);

    JS_FreeValue(ctx, pair);
    JS_FreeValue(ctx, key_string_val);
    JS_FreeValue(ctx, value_string_val);
    JS_FreeValue(ctx, pair_push_func);
  }

  JS_FreeValue(ctx, push_func);

  return arr;
}

static const JSCFunctionListEntry js_headers_proto_funcs[] = {
    JS_CFUNC_DEF("get", 1, api_headers_get),
    JS_CFUNC_DEF("set", 2, api_headers_set),
    JS_CFUNC_DEF("append", 2, api_headers_append),
    JS_CFUNC_DEF("delete", 1, api_headers_delete),
    JS_CFUNC_DEF("has", 1, api_headers_has),
    JS_CFUNC_DEF("keys", 0, api_headers_keys),
    JS_CFUNC_DEF("values", 0, api_headers_values),
    JS_CFUNC_DEF("entries", 0, api_headers_entries),
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