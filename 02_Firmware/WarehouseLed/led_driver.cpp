#include "led_driver.h"
#include "config.h"
bool LedDriver::begin() {
  for (size_t i = 0; i < 4; ++i) {
    // Fixed capacity also clears pixels left over after a mapping change.
    if (strips_[i].numPixels() != 75 || !strips_[i].begin()) return false;
  }
  ready_ = true; off();
  return true;
}
void LedDriver::off() {
  hardwareTests_.clear();
  if (!ready_) return;
  for (size_t i = 0; i < 4; ++i) { strips_[i].clear(); strips_[i].show(); }
  memset(last_, 0, sizeof(last_));
}
void LedDriver::update(const Warehouse::Controller& controller, uint32_t now) {
  if (!ready_) return;
  Warehouse::Pixel frame[75];
  for (size_t o = 0; o < 4; ++o) {
    controller.render(o, now, frame);
    hardwareTests_.render(o, controller.config(), now, frame);
    if (!memcmp(last_[o], frame, sizeof(frame))) continue;
    for (size_t p = 0; p < 75; ++p) strips_[o].setPixelColor(p, frame[p].r, frame[p].g, frame[p].b);
    strips_[o].show();
    memcpy(last_[o], frame, sizeof(frame));
  }
}
