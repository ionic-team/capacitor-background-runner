#ifndef CAPACITOR_BACKGROUND_RUNNER_API_BLOB_H
#define CAPACITOR_BACKGROUND_RUNNER_API_BLOB_H

#include <vector>

#include "../quickjs/quickjs.h"

class BlobBackingStore {
 public:
  uint8_t *data();
  size_t size();
  void append(std::vector<uint8_t> vec);
  std::string string();

 private:
  std::vector<std::vector<uint8_t>> storage;
};

void init_blob_class(JSContext *ctx);
JSValue new_blob(JSContext *ctx, uint8_t *data, size_t size);

#endif  // CAPACITOR_BACKGROUND_RUNNER_API_BLOB_H
