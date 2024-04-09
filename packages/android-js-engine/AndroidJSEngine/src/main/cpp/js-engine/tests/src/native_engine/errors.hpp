#ifndef ERRORS_H
#define ERRORS_H

#include <exception>
#include <string>

class JavaScriptException : public std::exception {
 public:
  JavaScriptException(const char* js_exception);
  const char* what() const throw();

 private:
  const char* js_exception;
  std::string message;
};

class FetchException : public std::exception {
 public:
  FetchException(const char* fetch_exception);
  const char* what() const throw();

 private:
  const char* fetch_exception;
  std::string message;
};

#endif  // ERRORS_H