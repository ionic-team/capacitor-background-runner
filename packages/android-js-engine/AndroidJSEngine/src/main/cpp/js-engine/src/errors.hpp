#ifndef CAPACITOR_BACKGROUND_RUNNER_ERRORS_H
#define CAPACITOR_BACKGROUND_RUNNER_ERRORS_H

#include <exception>
#include <string>
#include "quickjs/quickjs.h"

class NativeInterfaceException : public std::exception {
public:
    NativeInterfaceException(const char* error);
    const char* what() const throw();

private:
    std::string message;
};

JSValue create_js_error(const char * error_message, JSContext *ctx);
JSValue get_js_exception(JSContext *ctx);

void reject_promise(JSContext *ctx, JSValue reject_func, JSValue reject_obj);

#endif  // CAPACITOR_BACKGROUND_RUNNER_ERRORS_H