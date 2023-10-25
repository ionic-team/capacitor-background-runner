#ifndef CAPACITOR_BACKGROUND_NATIVE_H
#define CAPACITOR_BACKGROUND_NATIVE_H

#include <string>

enum LoggerLevel { INFO, WARN, ERROR, DEBUG };

class NativeInterface {
 public:
  virtual ~NativeInterface() = default;
  NativeInterface(const NativeInterface&) = delete;
  NativeInterface(NativeInterface&&) = delete;
  NativeInterface& operator=(const NativeInterface&) = delete;

  virtual void logger(LoggerLevel level, const std::string& tag, const std::string& messages) = 0;

 protected:
  NativeInterface() = default;
};

#endif  // CAPACITOR_BACKGROUND_NATIVE_H