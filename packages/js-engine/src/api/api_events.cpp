#include "api_events.h"

#include "../context.hpp"

JSValue api_add_event_listener(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  const char *event = JS_ToCString(ctx, argv[0]);

  JSValue callback = argv[1];

  if (!JS_IsFunction(ctx, callback)) {
    return JS_EXCEPTION;
  }

  callback = JS_DupValue(ctx, callback);

  auto *context = (Context *)JS_GetContextOpaque(ctx);
  auto itr = context->event_listeners.find(event);

  if (itr == context->event_listeners.end()) {
    context->event_listeners.emplace(event, callback);
  } else {
    JS_FreeValue(ctx, itr->second);
    itr->second = callback;
  }

  JS_FreeCString(ctx, event);

  return JS_UNDEFINED;
}
