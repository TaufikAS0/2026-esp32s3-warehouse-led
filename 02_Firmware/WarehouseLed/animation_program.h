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
  struct Cursor {
    int output = -1, box = -1;
    unsigned position = 0, total = 0;
    uint32_t phase = 0, dwell = 0;
  };
  void start(Pattern pattern, bool idle, uint8_t brightness, uint32_t step,
             uint32_t duration, const Config& config, uint32_t now) {
    pattern_ = pattern; idle_ = idle;
    brightness_ = brightness > config.brightnessLimit ? config.brightnessLimit : brightness;
    step_ = step < 100 ? 100 : step; duration_ = duration;
    tick_ = now; elapsed_ = 0; count_ = config.boxCount;
    for (size_t i = 0; i < count_; ++i) {
      size_t j = i;
      while (j && before(config.boxes[i], config.boxes[order_[j - 1]])) {
        order_[j] = order_[j - 1]; --j;
      }
      order_[j] = i;
    }
    memset(outputCounts_, 0, sizeof(outputCounts_));
    for (size_t i = 0; i < count_; ++i) ++outputCounts_[config.boxes[order_[i]].output];
    cycleMs_ = 0;
    for (size_t o = 0; o < kOutputs; ++o) if (outputCounts_[o]) cycleMs_ += dwell(o);
    enabled_ = cycleMs_ != 0;
  }
  void update(uint32_t now) {
    if (!enabled_) return;
    elapsed_ += uint32_t(now - tick_); tick_ = now;
    if (!idle_ && elapsed_ >= duration_) enabled_ = false;
  }
  void stop() { enabled_ = false; }
  bool active(uint32_t now) const { return enabled_ && (idle_ || elapsed(now) < duration_); }
  uint32_t remaining(uint32_t now) const { return active(now) && !idle_ ? duration_ - elapsed(now) : 0; }
  bool idle() const { return idle_; }
  Pattern pattern() const { return pattern_; }
  uint8_t brightness() const { return brightness_; }
  uint32_t step() const { return step_; }
  uint32_t cycleMs() const { return cycleMs_; }
  Cursor cursor(uint32_t now) const {
    Cursor c;
    if (!active(now) || !cycleMs_) return c;
    uint32_t phase = elapsed(now) % cycleMs_;
    unsigned offset = 0;
    for (size_t o = 0; o < kOutputs; ++o) {
      if (!outputCounts_[o]) continue;
      const uint32_t span = dwell(o);
      if (phase < span) {
        c.output = o; c.phase = phase; c.dwell = span; c.total = outputCounts_[o];
        if (pattern_ == Pattern::Running || pattern_ == Pattern::Blink) {
          c.position = phase / step_ + 1; c.box = order_[offset + c.position - 1];
        }
        return c;
      }
      phase -= span; offset += outputCounts_[o];
    }
    return c;
  }
  bool boxLit(size_t i, const Controller& controller, uint32_t now) const {
    if (i >= controller.config().boxCount || controller.remaining(i, now)) return false;
    const Cursor c = cursor(now);
    if (c.output != controller.config().boxes[i].output) return false;
    if (pattern_ == Pattern::Rainbow || pattern_ == Pattern::Breathe) return true;
    return c.box == int(i) && (pattern_ != Pattern::Blink || c.phase % step_ < step_ / 2);
  }
  Pixel pixelColor(size_t i, size_t pixel, const Config& config, uint32_t now) const {
    const auto& b = config.boxes[i];
    uint8_t r = b.color[0], g = b.color[1], blue = b.color[2];
    uint16_t level = brightness_;
    const Cursor c = cursor(now);
    if (c.output < 0) return Pixel{};
    if (pattern_ == Pattern::Rainbow) {
      const uint32_t phase = (uint64_t(c.phase / 20 * 20) * 768 / c.dwell +
                              pixel * 768 / config.ledCounts[b.output]) % 768;
      if (phase < 256) { r = 255 - phase; g = phase; blue = 0; }
      else if (phase < 512) { r = 0; g = 511 - phase; blue = phase - 256; }
      else { r = phase - 512; g = 0; blue = 767 - phase; }
    } else if (pattern_ == Pattern::Breathe) {
      const uint32_t phase = c.phase / 20 * 20;
      const uint32_t ramp = phase < step_ ? phase : step_ * 2 - phase;
      level = level * ramp / step_;
    }
    if (level > config.brightnessLimit) level = config.brightnessLimit;
    return Pixel{uint8_t(r * level / 255), uint8_t(g * level / 255), uint8_t(blue * level / 255)};
  }
  Pixel color(size_t i, const Config& config, uint32_t now) const {
    return pixelColor(i, config.boxes[i].start, config, now);
  }
  void render(size_t output, const Controller& controller, uint32_t now, Pixel* frame) const {
    const auto& config = controller.config();
    for (size_t i = 0; i < config.boxCount; ++i) {
      const auto& b = config.boxes[i];
      if (b.output != output || !boxLit(i, controller, now)) continue;
      for (size_t p = b.start; p < size_t(b.start) + b.count; ++p)
        frame[p] = pixelColor(i, p, config, now);
    }
  }
 private:
  static bool before(const Box& a, const Box& b) {
    return a.output < b.output || (a.output == b.output && a.start < b.start);
  }
  uint64_t elapsed(uint32_t now) const { return elapsed_ + uint32_t(now - tick_); }
  uint32_t dwell(size_t o) const {
    return step_ * (pattern_ == Pattern::Rainbow ? 6 : pattern_ == Pattern::Breathe ? 2 : outputCounts_[o]);
  }
  bool enabled_ = false, idle_ = false;
  Pattern pattern_ = Pattern::Running;
  uint8_t brightness_ = 32, order_[kMaxBoxes]{}, outputCounts_[kOutputs]{};
  size_t count_ = 0;
  uint32_t tick_ = 0, step_ = 500, duration_ = 30000, cycleMs_ = 0;
  uint64_t elapsed_ = 0;
};
}
