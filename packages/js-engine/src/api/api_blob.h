#ifndef CAPACITOR_BACKGROUND_RUNNER_API_BLOB_H
#define CAPACITOR_BACKGROUND_RUNNER_API_BLOB_H

#include "../quickjs/quickjs.h"

void init_blob_class(JSContext *ctx);
JSValue new_blob(JSContext *ctx, uint8_t *data, size_t size);

#endif //CAPACITOR_BACKGROUND_RUNNER_API_BLOB_H
