#pragma once
#include "domain.h"
namespace Warehouse {
enum class Pattern : uint8_t { Running, Blink, Rainbow, Breathe };
inline const char* patternName(Pattern p) {
  switch (p) {
    case Pattern::Blink: return "blink";
    case Pattern::Rainbow: return "rainbow";
    case Pattern::Breathe: return "breathe";
    default: return "running";
  }
}
class AnimationProgram {
 public:
  void start(Pattern pattern, bool idle, uint8_t brightness, uint32_t step,
             uint32_t duration, const Config& config, uint32_t now) {
    pattern_ = pattern; idle_ = idle; brightness_ = brightness > config.brightnessLimit ? config.brightnessLimit : brightness;
    step_ = step; duration_ = duration; started_ = now; enabled_ = true;
  }
  void stop() { enabled_ = false; }
  bool active(uint32_t now) const { return enabled_ && (idle_ || now - started_ < duration_); }
  uint32_t remaining(uint32_t now) const { return active(now) && !idle_ ? duration_ - (now - started_) : 0; }
  bool idle() const { return idle_; }
  Pattern pattern() const { return pattern_; }
  uint8_t brightness() const { return brightness_; }
  uint32_t step() const { return step_; }
  bool boxLit(size_t i, const Controller& controller, uint32_t now) const {
    if (!active(now) || i >= controller.config().boxCount || controller.remaining(i, now)) return false;
    const uint32_t elapsed = now - started_;
    if (pattern_ == Pattern::Rainbow || pattern_ == Pattern::Breathe) return true;
    if (i != (elapsed / step_) % controller.config().boxCount) return false;
    return pattern_ != Pattern::Blink || elapsed % step_ < step_ / 2;
  }
  Pixel color(size_t i, const Config& config, uint32_t now) const {
    const auto& b = config.boxes[i];
    uint8_t r = b.color[0], g = b.color[1], blue = b.color[2];
    uint16_t level = brightness_;
    const uint32_t elapsed = ((now - started_) / 20) * 20; // Limit changing color frames to50 Hz.
    if (pattern_ == Pattern::Rainbow) {
      const uint16_t phase = ((elapsed / 30) + i * 37) % 768;
      if (phase < 256) { r = 255 - phase; g = phase; blue = 0; }
      else if (phase < 512) { r = 0; g = 511 - phase; blue = phase - 256; }
      else { r = phase - 512; g = 0; blue = 767 - phase; }
    } else if (pattern_ == Pattern::Breathe) {
      const uint32_t phase = elapsed % (step_ * 2);
      const uint32_t ramp = phase < step_ ? phase : step_ * 2 - phase;
      level = level * ramp / step_;
    }
    if (level > config.brightnessLimit) level = config.brightnessLimit;
    return Pixel{uint8_t(r * level / 255), uint8_t(g * level / 255), uint8_t(blue * level / 255)};
  }
  void render(size_t output, const Controller& controller, uint32_t now, Pixel* frame) const {
    const auto& config = controller.config();
    for (size_t i = 0; i < config.boxCount; ++i) {
      const auto& b = config.boxes[i];
      if (b.output != output || !boxLit(i, controller, now)) continue;
      const Pixel value = color(i, config, now);
      for (size_t p = b.start; p < size_t(b.start) + b.count; ++p) frame[p] = value;
    }
  }
 private:
  bool enabled_ = false, idle_ = false;
  Pattern pattern_ = Pattern::Running;
  uint8_t brightness_ = 32;
  uint32_t started_ = 0, step_ = 500, duration_ = 30000;
};
}
