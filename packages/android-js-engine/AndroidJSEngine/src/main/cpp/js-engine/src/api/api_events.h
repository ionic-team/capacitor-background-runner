#ifndef CAPACITOR_BACKGROUND_RUNNER_API_EVENTS_H
#define CAPACITOR_BACKGROUND_RUNNER_API_EVENTS_H

#include "../quickjs/quickjs.h"

JSValue api_add_event_listener(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);

#endif //CAPACITOR_BACKGROUND_RUNNER_API_EVENTS_H
