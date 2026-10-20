#include "animation_program.h"
#include <cassert>
#include <iostream>
using namespace Warehouse;
int main() {
  Controller controller; auto config = controller.config();
  config.boxCount = 4; config.ledCounts[0] = 6; config.ledCounts[1] = 0; config.ledCounts[2] = 3; config.ledCounts[3] = 3;
  // Deliberately shuffled storage order and misleading IDs.
  const uint8_t outputs[] = {3, 0, 2, 0}, starts[] = {0, 3, 0, 0};
  for (unsigned i = 0; i < 4; ++i) {
    auto& b = config.boxes[i]; snprintf(b.id, sizeof(b.id), "custom-%u", 9 - i);
    b.output = outputs[i]; b.start = starts[i]; b.count = 3;
    b.color[0] = 76; b.color[1] = 215; b.color[2] = 246;
  }
  controller.configure(config); AnimationProgram program; Pixel frame[4][75]{};
  auto render = [&](uint32_t now) {
    program.update(now);
    for (size_t o = 0; o < 4; ++o) {
      memset(frame[o], 0, sizeof(frame[o])); program.render(o, controller, now, frame[o]);
    }
    size_t lit = 0, racks = 0;
    for (const auto& rack : frame) { bool on = false; for (auto p : rack) if (p.r || p.g || p.b) { ++lit; on = true; } racks += on; }
    assert(racks <= 1 && lit <= 75);
  };
  program.start(Pattern::Running, true, 255, 500, 1000, config, 0);
  assert(program.cycleMs() == 2000);
  const int expected[] = {3, 1, 2, 0, 3};
  for (unsigned i = 0; i < 5; ++i) { render(i * 500); assert(program.cursor(i * 500).box == expected[i]); }
  assert(!strcmp(controller.config().boxes[0].id, "custom-9")); // source order unchanged
  program.start(Pattern::Blink, true, 32, 500, 1000, config, 0);
  render(249); assert(frame[0][0].g); render(250); assert(!frame[0][0].g);
  render(500); assert(frame[0][3].g && !frame[0][0].g);
  program.start(Pattern::Breathe, true, 128, 500, 1000, config, 0);
  render(0); assert(!frame[0][0].g); render(500);
  assert(frame[0][0].r == 76 * 128 / 255 && frame[0][0].g == 215 * 128 / 255 && frame[0][0].b == 246 * 128 / 255);
  render(1000); assert(program.cursor(1000).output == 2);
  render(1500); assert(frame[2][0].b);
  program.start(Pattern::Rainbow, true, 128, 500, 1000, config, 0);
  render(0); assert(frame[0][0].r == 128 && !frame[0][0].g);
  assert(frame[0][2].g == 128); // physical position, not config row
  render(3000); assert(program.cursor(3000).output == 2);
  render(6000); assert(program.cursor(6000).output == 3);
  render(9000); assert(program.cursor(9000).output == 0);
  program.start(Pattern::Running, false, 32, 500, 1000, config, UINT32_MAX - 100);
  render(399); assert(program.active(399)); render(899); assert(!program.active(899));
  render(UINT32_MAX - 100); assert(!program.active(UINT32_MAX - 100)); // no resurrection after full wrap
  program.start(Pattern::Running, true, 32, 500, 1000, config, 0);
  render(UINT32_MAX - 1); render(1000); assert(program.active(1000));
  program.stop(); render(1500); assert(!program.active(1500));
  std::cout << "Animation tests passed: physical ordering, exclusive racks, spectrum, saved RGB, expiry, long uptime\n";
}
