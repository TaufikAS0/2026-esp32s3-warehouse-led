#pragma once
#include "Arduino.h"
#include <functional>
using ota_error_t = unsigned;
struct OtaStub {
  bool enabled = false;
  void setHostname(const char*) {}
  void setPassword(const char*) {}
  void setTimeout(unsigned) {}
  void onStart(std::function<void()>) {}
  void onEnd(std::function<void()>) {}
  void onError(std::function<void(ota_error_t)>) {}
  void begin() { enabled = true; }
  void end() { enabled = false; }
  void handle() {}
};
inline OtaStub ArduinoOTA;
