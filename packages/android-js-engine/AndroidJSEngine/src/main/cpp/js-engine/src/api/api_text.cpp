#include "api_text.h"

#include "../context.hpp"
#include "../errors.hpp"
#include "../quickjs/cutils.h"

static JSClassID js_text_encoder_class_id;
static JSClassID js_text_decoder_class_id;

static JSClassDef js_text_encoder_class = {
    "TextEncoder",
};

static JSClassDef js_text_decoder_class = {
    "TextDecoder",
};

static JSValue api_text_encoder_constructor(JSContext *ctx, JSValueConst new_target, int argc, JSValueConst *argv) {
  JSValue const proto = JS_GetClassProto(ctx, js_text_encoder_class_id);

  auto new_object_class = JS_NewObjectProtoClass(ctx, proto, js_text_encoder_class_id);

  JS_FreeValue(ctx, proto);

  return new_object_class;
}

static JSValue api_text_decoder_constructor(JSContext *ctx, JSValueConst new_target, int argc, JSValueConst *argv) {
  JSValue const proto = JS_GetClassProto(ctx, js_text_decoder_class_id);

  auto new_object_class = JS_NewObjectProtoClass(ctx, proto, js_text_decoder_class_id);

  JS_FreeValue(ctx, proto);

  if (JS_IsException(new_object_class)) {
    return new_object_class;
  }

  JS_SetPropertyStr(ctx, new_object_class, "label", JS_NewString(ctx, "utf-8"));

  if (argc == 1) {
    if (JS_IsString(argv[0])) {
      JS_SetPropertyStr(ctx, new_object_class, "label", argv[0]);
    }
  }

  return new_object_class;
}

static JSValue api_text_encoder_encode(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  if (!JS_IsString(argv[0])) {
    return JS_EXCEPTION;
  }

  auto *context = (Context *)JS_GetContextOpaque(ctx);
  if (context == nullptr) {
    auto js_error = create_js_error("parent context is null", ctx);
    return JS_Throw(ctx, js_error);
  }

  try {
    const auto *c_str = JS_ToCString(ctx, argv[0]);
    auto byte_array = context->native_interface->string_to_byte_array(std::string(c_str));

    uint8_t *buf = new uint8_t[byte_array.size()];

    for (int i = 0; i < byte_array.size(); i++) {
      buf[i] = byte_array.at(i);
    }

    auto array_buffer = JS_NewArrayBuffer(ctx, buf, byte_array.size(), nullptr, nullptr, 0);
    auto ret_value = JS_NewUInt8Array(ctx, array_buffer, 0, byte_array.size());

    JS_FreeCString(ctx, c_str);
    JS_FreeValue(ctx, array_buffer);

    return ret_value;
  } catch (std::exception &ex) {
    auto js_error = create_js_error(ex.what(), ctx);
    return JS_Throw(ctx, js_error);
  }
}

static JSValue api_text_decoder_decode(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  uint8_t *buf;
  size_t elem, len, offset, buf_size;

  auto t_arr_buf = JS_GetTypedArrayBuffer(ctx, argv[0], &offset, &len, &elem);
  buf = JS_GetArrayBuffer(ctx, &buf_size, t_arr_buf);

  if (!buf) {
    JS_FreeValue(ctx, t_arr_buf);
    return JS_EXCEPTION;
  }

  JS_FreeValue(ctx, t_arr_buf);

  auto *context = (Context *)JS_GetContextOpaque(ctx);
  if (context == nullptr) {
    auto js_error = create_js_error("parent context is null", ctx);
    return JS_Throw(ctx, js_error);
  }

  auto label_prop_str = JS_GetPropertyStr(ctx, this_val, "label");
  const auto *encoding = JS_ToCString(ctx, label_prop_str);

  try {
    auto str = context->native_interface->byte_array_to_str(buf, buf_size, std::string(encoding));
    auto ret_value = JS_NewString(ctx, str.c_str());

    JS_FreeCString(ctx, encoding);
    return ret_value;
  } catch (std::exception &ex) {
    JS_FreeCString(ctx, encoding);

    auto js_error = create_js_error(ex.what(), ctx);
    return JS_Throw(ctx, js_error);
  }
}

static const JSCFunctionListEntry js_text_encoder_proto_funcs[] = {
    JS_CFUNC_DEF("encode", 1, api_text_encoder_encode),
};

static const JSCFunctionListEntry js_text_decoder_proto_funcs[] = {
    JS_CFUNC_DEF("decode", 1, api_text_decoder_decode),
};

void init_text_encoder_class(JSContext *ctx) {
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
