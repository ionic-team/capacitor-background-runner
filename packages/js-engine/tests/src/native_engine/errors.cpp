#include "errors.hpp"

JavaScriptException::JavaScriptException(const char* js_exception) {
  this->js_exception = js_exception;
  this->message = "JS Exception: " + std::string(js_exception);
}

const char* JavaScriptException::what() const throw() { return this->message.c_str(); }