#include "api_events.h"

#include "context.h"

JSValue api_add_event_listener(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  const char *event = JS_ToCString(ctx, argv[0]);

  JSValue callback = argv[1];

  if (!JS_IsFunction(ctx, callback)) {
    return JS_EXCEPTION;
  }

  callback = JS_DupValue(ctx, callback);

  auto *parent_ctx = (Context *)JS_GetContextOpaque(ctx);
  parent_ctx->event_listeners.emplace(event, callback);

  JS_FreeCString(ctx, event);

  return JS_UNDEFINED;
}