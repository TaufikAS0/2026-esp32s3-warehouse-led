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
  program_.stop();
  if (!ready_) return;
  for (size_t i = 0; i < 4; ++i) { strips_[i].clear(); strips_[i].show(); }
  memset(last_, 0, sizeof(last_));
}
void LedDriver::update(const Warehouse::Controller& controller, uint32_t now) {
  if (!ready_) return;
  program_.update(now); hardwareTests_.update(now);
  Warehouse::Pixel frames[4][75]{};
  const bool hardware = hardwareTests_.active(now);
  const bool automatic = program_.active(now) || hardware;
  for (size_t o = 0; o < 4; ++o) {
    if (!automatic) controller.render(o, now, frames[o]);
    if (!hardware && program_.active(now)) program_.render(o, controller, now, frames[o]);
    if (hardware) hardwareTests_.render(o, controller.config(), now, frames[o]);
  }
  // Clear all retiring outputs before lighting any new output (including 4 -> 1).
  const Warehouse::Pixel black[75]{};
  for (size_t o = 0; o < 4; ++o) {
    if (!memcmp(frames[o], black, sizeof(black)) && memcmp(last_[o], black, sizeof(black))) {
      strips_[o].clear(); strips_[o].show(); memcpy(last_[o], black, sizeof(black));
    }
  }
  for (size_t o = 0; o < 4; ++o) {
    if (!memcmp(last_[o], frames[o], sizeof(frames[o]))) continue;
    for (size_t p = 0; p < 75; ++p) strips_[o].setPixelColor(p, frames[o][p].r, frames[o][p].g, frames[o][p].b);
    strips_[o].show();
    memcpy(last_[o], frames[o], sizeof(frames[o]));
  }
}
