// Uses the real pinned NeoPixel C++ implementation. Only the Arduino GPIO
// and ESP32 RMT boundary are replaced: this does not simulate electrical timing.
#include "led_driver.h"
#include <cassert>
#include <iostream>
#include <vector>
static unsigned initCalls = 0;
static unsigned sent[8]{};
static uint8_t wire[8][225]{};
static bool exclusive = false;
static std::vector<uint16_t> events;
extern "C" void espInit() { ++initCalls; }
extern "C" void espShow(uint16_t pin, uint8_t* pixels, uint32_t bytes, bool is800) {
  // Model esp.c's mutex gate: no initialization means no transmission.
  if (!initCalls) return;
  assert(pin < 8 && bytes == 225 && is800);
  ++sent[pin]; memcpy(wire[pin], pixels, bytes);
  events.push_back(pin);
  if (exclusive) {
    unsigned outputs = 0, lit = 0;
    for (auto p : Hardware::kLedPins) {
      bool on = false;
      for (unsigned pixel = 0; pixel < 75; ++pixel)
        if (wire[p][pixel * 3] || wire[p][pixel * 3 + 1] || wire[p][pixel * 3 + 2]) { ++lit; on = true; }
      outputs += on;
    }
    assert(outputs <= 1 && lit <= 75); // Check after EVERY show, not just final frame.
  }
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
  cmd.color[0] = 76; cmd.color[1] = 215; cmd.color[2] = 246; cmd.brightness = 128;
  controller.set(0, cmd, 102); driver.update(controller, 102);
  assert(wire[4][0] == 107 && wire[4][1] == 38 && wire[4][2] == 123); // cyan RGB -> actual GRB bytes
  auto config = controller.config(); config.ledCounts[3] = 75;
  cmd.color[0] = 255; cmd.color[1] = 0; cmd.color[2] = 0; cmd.brightness = 50;
  driver.hardwareTests().set(3, cmd, config, 100);
  controller.configure(config);
  driver.update(controller, 101);
  assert(wire[7][1] == 50 && wire[7][223] == 50); // GPIO7, last of75 red.
  driver.off();
  for (auto pin : Hardware::kLedPins) for (auto byte : wire[pin]) assert(byte == 0);
  config.boxCount = 2; config.ledCounts[0] = 75; config.ledCounts[3] = 75;
  config.boxes[0].output = 3; config.boxes[0].start = 0; config.boxes[0].count = 75;
  config.boxes[1] = config.boxes[0]; config.boxes[1].output = 0;
  strcpy(config.boxes[1].id, "custom-other"); controller.configure(config);
  exclusive = true;
  driver.program().start(Warehouse::Pattern::Rainbow, true, 128, 100, 1000, config, 0);
  driver.update(controller, 0); driver.update(controller, 600);
  events.clear(); driver.update(controller, 1200);
  assert(events.size() == 2 && events[0] == 7 && events[1] == 4); // output4 black BEFORE output1 light
  driver.off();
  driver.hardwareTests().set(3, cmd, config, 2000); driver.update(controller, 2000);
  driver.hardwareTests().set(0, cmd, config, 2001); events.clear(); driver.update(controller, 2001);
  assert(events[0] == 7 && events[1] == 4);
  driver.off();
  std::cout << "Real NeoPixel initialization and driver boundary tests passed\n";
}
