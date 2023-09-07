#ifndef CAPACITOR_BACKGROUND_RUNNER_API_JS_RESPONSE_H
#define CAPACITOR_BACKGROUND_RUNNER_API_JS_RESPONSE_H

#include "../quickjs/quickjs.h"
#include <jni.h>

void init_response_class(JSContext *ctx);
JSValue new_js_response(JSContext *ctx, jobject j_response);

#endif //CAPACITOR_BACKGROUND_RUNNER_API_JS_RESPONSE_H
