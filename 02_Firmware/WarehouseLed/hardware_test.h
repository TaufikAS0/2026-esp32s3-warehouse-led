#pragma once
#include "domain.h"
namespace Warehouse {
class HardwareTests {
 public:
  void set(size_t output, const Command& command, const Config& config, uint32_t now) {
    if (output >= kOutputs) return;
    states_[output] = State{};
    static_cast<Command&>(states_[output]) = command;
    if (states_[output].brightness > config.brightnessLimit) states_[output].brightness = config.brightnessLimit;
    states_[output].startedMs = now;
  }
  uint32_t remaining(size_t output, uint32_t now) const {
    if (output >= kOutputs || !states_[output].on) return 0;
    const uint32_t elapsed = now - states_[output].startedMs;
    return elapsed < states_[output].durationMs ? states_[output].durationMs - elapsed : 0;
  }
  const State& state(size_t output) const { return states_[output]; }
  void cancel(size_t output) { if (output < kOutputs) states_[output].on = false; }
  void clear() { for (auto& state : states_) state.on = false; }
  bool render(size_t output, const Config& config, uint32_t now, Pixel* frame) const {
    if (!remaining(output, now)) return false;
    memset(frame, 0, sizeof(Pixel) * kLedsPerOutput);
    const State& s = states_[output];
    if (s.blink && ((now - s.startedMs) % 1000 >= 500)) return true;
    for (size_t i = 0; i < config.ledCounts[output]; ++i)
      frame[i] = Pixel{uint8_t(uint16_t(s.color[0]) * s.brightness / 255),
                       uint8_t(uint16_t(s.color[1]) * s.brightness / 255),
                       uint8_t(uint16_t(s.color[2]) * s.brightness / 255)};
    return true;
  }
 private:
  State states_[kOutputs]{};
};
}
