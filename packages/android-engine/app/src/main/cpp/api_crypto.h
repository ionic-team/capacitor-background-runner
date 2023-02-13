#include "quickjs/quickjs.h"

#ifndef ANDROID_ENGINE_API_CRYPTO_H
#define ANDROID_ENGINE_API_CRYPTO_H

JSValue api_crypto_get_random_values(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);
JSValue api_crypto_random_uuid(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);

#endif //ANDROID_ENGINE_API_CRYPTO_H
