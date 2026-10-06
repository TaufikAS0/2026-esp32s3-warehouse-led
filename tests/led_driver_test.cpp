// Uses the real pinned NeoPixel C++ implementation. Only the Arduino GPIO
// and ESP32 RMT boundary are replaced: this does not simulate electrical timing.
#include "led_driver.h"
#include <cassert>
#include <iostream>
static unsigned initCalls = 0;
static unsigned sent[8]{};
static uint8_t wire[8][225]{};
extern "C" void espInit() { ++initCalls; }
extern "C" void espShow(uint16_t pin, uint8_t* pixels, uint32_t bytes, bool is800) {
  // Model esp.c's mutex gate: no initialization means no transmission.
  if (!initCalls) return;
  assert(pin < 8 && bytes == 225 && is800);
  ++sent[pin]; memcpy(wire[pin], pixels, bytes);
}
int main() {
  {
    Adafruit_NeoPixel old;
    old.setPin(4); old.updateType(NEO_GRB + NEO_KHZ800); old.updateLength(75);
    assert(old.begin()); old.setPixelColor(0, 0, 255, 0); old.show();
    assert(initCalls == 0 && sent[4] == 0); // Reproduce previous silent failure.
  }
  LedDriver driver;
  assert(initCalls == 4);
  assert(driver.begin() && driver.ready());
  for (auto pin : Hardware::kLedPins) assert(sent[pin] == 1);
  Warehouse::Controller controller;
  Warehouse::Command cmd; cmd.on = true; cmd.brightness = 50;
  assert(controller.set(0, cmd, 100));
  driver.update(controller, 100);
  assert(sent[4] == 2 && wire[4][0] == 50 && wire[4][1] == 0 && wire[4][2] == 0);
  assert(wire[4][6] == 50 && wire[4][9] == 0); // Three green pixels; tail clear.
  driver.update(controller, 101); assert(sent[4] == 2); // No unchanged frames.
  auto config = controller.config(); config.ledCounts[3] = 75;
  cmd.color[0] = 255; cmd.color[1] = 0;
  driver.hardwareTests().set(3, cmd, config, 100);
  controller.configure(config);
  driver.update(controller, 101);
  assert(wire[7][1] == 50 && wire[7][223] == 50); // GPIO7, last of75 red.
  driver.off();
  for (auto pin : Hardware::kLedPins) for (auto byte : wire[pin]) assert(byte == 0);
  std::cout << "Real NeoPixel initialization and driver boundary tests passed\n";
}
