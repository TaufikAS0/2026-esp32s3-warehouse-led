#pragma once
#include <Adafruit_NeoPixel.h>
#include "domain.h"
#include "hardware_test.h"
#include "config.h"
class LedDriver {
 public:
  bool begin();
  void update(const Warehouse::Controller& controller, uint32_t now);
  void off();
  bool ready() const { return ready_; }
  Warehouse::HardwareTests& hardwareTests() { return hardwareTests_; }
 private:
  // The parameterized constructor initializes ESP32's shared RMT mutex.
  // The deprecated empty constructor + updateLength/setPin does not (NeoPixel 1.15.2).
  Adafruit_NeoPixel strips_[4] = {
      {75, Hardware::kLedPins[0], NEO_GRB + NEO_KHZ800},
      {75, Hardware::kLedPins[1], NEO_GRB + NEO_KHZ800},
      {75, Hardware::kLedPins[2], NEO_GRB + NEO_KHZ800},
      {75, Hardware::kLedPins[3], NEO_GRB + NEO_KHZ800}};
  Warehouse::Pixel last_[4][75]{};
  bool ready_ = false;
  Warehouse::HardwareTests hardwareTests_;
};
