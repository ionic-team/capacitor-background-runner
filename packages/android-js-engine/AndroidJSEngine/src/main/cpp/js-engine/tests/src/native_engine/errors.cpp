#include "errors.hpp"

JavaScriptException::JavaScriptException(const char* js_exception) {
  this->js_exception = js_exception;
  this->message = "JS Exception: " + std::string(js_exception);
}

FetchException::FetchException(const char* fetch_exception) {
  this->fetch_exception = fetch_exception;
  this->message = "Fetch Exception: " + std::string(fetch_exception);
}

const char* JavaScriptException::what() const throw() { return this->message.c_str(); }
const char* FetchException::what() const throw() { return this->message.c_str(); }