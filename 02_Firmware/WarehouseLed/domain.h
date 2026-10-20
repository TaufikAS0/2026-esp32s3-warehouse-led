#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

namespace Warehouse {
constexpr size_t kOutputs = 4;
constexpr size_t kLedsPerOutput = 75;
constexpr size_t kMaxBoxes = 100;
struct Box {
  char id[25]{};
  char name[49]{};
  uint8_t output = 0; // Internal zero-based output.
  uint8_t start = 0;  // Internal zero-based LED.
  uint8_t count = 3;
  uint8_t color[3] = {0, 255, 0}; // Saved display/animation color; commands remain transient.
};
struct Config {
  uint8_t ledCounts[kOutputs] = {3, 0, 0, 0};
  uint8_t brightnessLimit = 128;
  uint16_t boxCount = 1;
  Box boxes[kMaxBoxes]{};
};
struct Command {
  bool on = false;
  uint8_t color[3] = {0, 255, 0};
  uint8_t brightness = 128;
  bool blink = false;
  uint32_t durationMs = 60000;
};
struct State : Command { uint32_t startedMs = 0; };
struct Pixel { uint8_t r = 0, g = 0, b = 0; };

inline void setDefaults(Config& c) {
  c.boxes[0] = Box{};
  c.ledCounts[0] = 3;
  for (size_t o = 1; o < kOutputs; ++o) c.ledCounts[o] = 0;
  c.brightnessLimit = 128; c.boxCount = 1;
  c.boxes[0].output = 0; c.boxes[0].start = 0; c.boxes[0].count = 3;
  strcpy(c.boxes[0].id, "R01-B01");
  strcpy(c.boxes[0].name, "Rak 1 Box 1");
}
inline Config defaults() {
  Config c; setDefaults(c);
  return c;
}
inline bool validId(const char* id) {
  const size_t n = strlen(id);
  if (!n || n > 24) return false;
  for (size_t i = 0; i < n; ++i) {
    const char x = id[i];
    if (!((x >= 'A' && x <= 'Z') || (x >= 'a' && x <= 'z') ||
          (x >= '0' && x <= '9') || x == '-' || x == '_')) return false;
  }
  return true;
}
inline const char* validate(const Config& c) {
  if (!c.boxCount || c.boxCount > kMaxBoxes) return "boxes must contain 1..100 entries";
  bool occupied[kOutputs][kLedsPerOutput]{};
  for (size_t o = 0; o < kOutputs; ++o)
    if (c.ledCounts[o] > kLedsPerOutput) return "output LED count exceeds 75";
  for (size_t i = 0; i < c.boxCount; ++i) {
    const Box& b = c.boxes[i];
    if (!validId(b.id)) return "invalid box ID";
    for (size_t j = 0; j < i; ++j)
      if (!strcmp(b.id, c.boxes[j].id)) return "duplicate box ID";
    if (b.output >= kOutputs || !b.count ||
        static_cast<unsigned>(b.start) + b.count > c.ledCounts[b.output])
      return "box exceeds installed LED range";
    for (size_t p = b.start; p < static_cast<size_t>(b.start) + b.count; ++p) {
      if (occupied[b.output][p]) return "overlapping box mapping";
      occupied[b.output][p] = true;
    }
  }
  return nullptr;
}

class Controller {
 public:
  Controller() { setDefaults(config_); }
  const Config& config() const { return config_; }
  const State& state(size_t i) const { return states_[i]; }
  int find(const char* id) const {
    for (size_t i = 0; i < config_.boxCount; ++i)
      if (!strcmp(id, config_.boxes[i].id)) return static_cast<int>(i);
    return -1;
  }
  void configure(const Config& c) { allOff(); config_ = c; }
  void allOff() { for (auto& s : states_) s.on = false; }
  bool set(size_t i, const Command& cmd, uint32_t now) {
    if (i >= config_.boxCount || (cmd.on && (cmd.durationMs < 1000 || cmd.durationMs > 3600000)))
      return false;
    static_cast<Command&>(states_[i]) = cmd;
    if (states_[i].brightness > config_.brightnessLimit)
      states_[i].brightness = config_.brightnessLimit;
    states_[i].startedMs = now;
    return true;
  }
  uint32_t remaining(size_t i, uint32_t now) const {
    const auto& s = states_[i];
    const uint32_t elapsed = now - s.startedMs;
    return s.on && elapsed < s.durationMs ? s.durationMs - elapsed : 0;
  }
  void update(uint32_t now) {
    for (size_t i = 0; i < config_.boxCount; ++i)
      if (states_[i].on && !remaining(i, now)) states_[i].on = false;
  }
  void render(size_t output, uint32_t now, Pixel* pixels) const {
    for (size_t p = 0; p < kLedsPerOutput; ++p) pixels[p] = Pixel{};
    for (size_t i = 0; i < config_.boxCount; ++i) {
      const auto& b = config_.boxes[i];
      const auto& s = states_[i];
      if (b.output != output || !remaining(i, now) ||
          (s.blink && ((now - s.startedMs) % 1000 >= 500))) continue;
      Pixel value;
      value.r = static_cast<uint16_t>(s.color[0]) * s.brightness / 255;
      value.g = static_cast<uint16_t>(s.color[1]) * s.brightness / 255;
      value.b = static_cast<uint16_t>(s.color[2]) * s.brightness / 255;
      for (size_t p = b.start; p < static_cast<size_t>(b.start) + b.count; ++p) pixels[p] = value;
    }
  }
 private:
  Config config_;
  State states_[kMaxBoxes]{};
};
// Shared by real persistence and host tests: no runtime mutation on failed write.
template<class Store>
bool commitConfig(Controller& controller, const Config& candidate, Store& store) {
  if (validate(candidate) || !store.save(candidate)) return false;
  controller.configure(candidate);
  return true;
}
} // namespace Warehouse
