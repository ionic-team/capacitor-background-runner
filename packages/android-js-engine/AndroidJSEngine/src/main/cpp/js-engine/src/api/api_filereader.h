#ifndef CAPACITOR_BACKGROUND_RUNNER_API_FILE_READER_H
#define CAPACITOR_BACKGROUND_RUNNER_API_FILE_READER_H
#include <string>
#include <unordered_map>

#include "../quickjs/quickjs.h"

class FileReaderStore {
 public:
  JSContext *ctx;
  std::unordered_map<std::string, JSValue> event_listeners;

  FileReaderStore(JSContext *ctx);
  ~FileReaderStore();
};

void init_file_reader_class(JSContext *ctx);

#endif  // CAPACITOR_BACKGROUND_RUNNER_API_FILE_READER_H