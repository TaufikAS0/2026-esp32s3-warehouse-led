#include "animation_program.h"
#include <cassert>
#include <iostream>
using namespace Warehouse;
int main() {
  Controller controller; auto config = controller.config();
  config.ledCounts[0] = 6; config.boxCount = 2;
  config.boxes[1] = config.boxes[0]; strcpy(config.boxes[1].id, "R01-B02");
  config.boxes[1].start = 3; config.boxes[1].color[0] = 255; config.boxes[1].color[1] = 0;
  controller.configure(config); AnimationProgram program; Pixel frame[75];
  auto render = [&](uint32_t now) { controller.render(0, now, frame); program.render(0, controller, now, frame); };
  program.start(Pattern::Running, false, 255, 500, 1000, config, UINT32_MAX - 100);
  render(UINT32_MAX - 100); assert(frame[0].g == 128 && frame[3].r == 0 && frame[6].r == 0);
  render(399); assert(frame[0].g == 0 && frame[3].r == 128); // rollover +500
  render(899); assert(!program.active(899) && frame[3].r == 0);
  program.start(Pattern::Blink, true, 32, 500, 1000, config, 0);
  render(249); assert(frame[0].g == 32); render(250); assert(frame[0].g == 0);
  render(500); assert(frame[3].r == 32);
  assert(program.active(100000) && program.remaining(100000) == 0);
  Command manual; manual.on = true; manual.blink = true; manual.color[1] = 0; manual.color[2] = 255;
  manual.brightness = 50; manual.durationMs = 1000; controller.set(0, manual, 0);
  program.start(Pattern::Rainbow, true, 32, 500, 1000, config, 0);
  render(100); assert(frame[0].b == 50 && frame[0].r == 0); // manual has priority
  render(600); assert(frame[0].r == 0 && frame[0].b == 0); // blink OFF remains black
  render(1100); assert(program.boxLit(0, controller, 1100)); // resumes after expiry
  controller.allOff(); program.start(Pattern::Breathe, false, 64, 500, 2000, config, 0);
  render(0); assert(frame[0].g == 0); render(500); assert(frame[0].g == 64);
  render(1000); assert(frame[0].g == 0); program.stop(); render(1500); assert(frame[0].g == 0);
  std::cout << "Animation tests passed: mapping, saved colors, expiry, idle, rollover, manual/blink priority\n";
}
