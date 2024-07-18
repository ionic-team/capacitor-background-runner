#ifndef CAPACITOR_BACKGROUND_RUNNER_API_HEADERS_H
#define CAPACITOR_BACKGROUND_RUNNER_API_HEADERS_H

#include <string>
#include <unordered_map>

#include "../quickjs/quickjs.h"

class HeaderBackingStore {
 public:
  void set(std::string name, std::string value);
  std::string get(std::string name);
  void remove(std::string name);
  bool exists(std::string name);
  void append(std::string name, std::string value);
  std::vector<std::string> keys();
  std::vector<std::string> values();

 private:
  std::unordered_map<std::string, std::string> storage;
};

void init_headers_class(JSContext *ctx);

#endif  // CAPACITOR_BACKGROUND_RUNNER_API_HEADERS_H