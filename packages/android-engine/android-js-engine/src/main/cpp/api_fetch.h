#include <jni.h>
#include "quickjs/quickjs.h"

#ifndef CAPACITOR_BACKGROUND_RUNNER_API_FETCH_H
#define CAPACITOR_BACKGROUND_RUNNER_API_FETCH_H

struct FetchPromise {
    JSValue resolve;
    JSValue reject;
    JSValue request;
    JSValue options;
};

JSValue js_response_to_value(JSContext *ctx, jobject response);
JSValue api_fetch_promise(JSContext *ctx, JSValueConst this_val, int argc, JSValue *argv);
void api_fetch(JSContext *ctx, FetchPromise promise);

#endif //CAPACITOR_BACKGROUND_RUNNER_API_FETCH_H
