#pragma once
#include <Adafruit_NeoPixel.h>
#include "domain.h"
class LedDriver {
 public:
  bool begin();
  void update(const Warehouse::Controller& controller, uint32_t now);
  void off();
  bool ready() const { return ready_; }
 private:
  Adafruit_NeoPixel strips_[4];
  Warehouse::Pixel last_[4][75]{};
  bool ready_ = false;
};
