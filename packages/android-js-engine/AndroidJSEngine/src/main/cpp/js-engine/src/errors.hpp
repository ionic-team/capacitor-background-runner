#ifndef CAPACITOR_BACKGROUND_RUNNER_ERRORS_H
#define CAPACITOR_BACKGROUND_RUNNER_ERRORS_H

#include "quickjs/quickjs.h"

JSValue throw_js_exception(JSContext *ctx, const char *message);
void reject_promise(JSContext *ctx, JSValue reject_func, JSValue reject_obj);

#endif  // CAPACITOR_BACKGROUND_RUNNER_ERRORS_H