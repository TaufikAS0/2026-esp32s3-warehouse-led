#include "domain.h"
#include "hardware_test.h"
#include <cassert>
#include <iostream>
#include <limits>
using namespace Warehouse;
struct FakeStore {
  bool succeed = true;
  unsigned writes = 0;
  Config saved{};
  bool save(const Config& c) { ++writes; if (succeed) saved = c; return succeed; }
};
int main() {
  HardwareTests tests; Config config = defaults(); Pixel testFrame[75];
  Command test; test.on = true; test.color[0] = 255; test.color[1] = 0;
  test.brightness = 255; test.durationMs = 3000;
  tests.set(0, test, config, UINT32_MAX - 1000);
  assert(tests.state(0).brightness == 128);
  assert(tests.render(0, config, UINT32_MAX - 1000, testFrame));
  assert(testFrame[0].r == 128 && testFrame[2].r == 128 && testFrame[3].r == 0);
  assert(tests.remaining(0, 999) == 1000); // rollover elapsed = 2000 ms
  assert(!tests.render(0, config, 1999, testFrame));
  test.blink = true; tests.set(0, test, config, 10);
  assert(tests.render(0, config, 510, testFrame) && testFrame[0].r == 0);
  assert(tests.render(0, config, 1010, testFrame) && testFrame[0].r == 128);
  config.ledCounts[1] = 75; tests.set(1, test, config, 10);
  tests.cancel(0); assert(!tests.remaining(0, 10) && tests.remaining(1, 10));
  assert(tests.render(1, config, 10, testFrame) && testFrame[74].r == 128);
  tests.clear(); assert(!tests.remaining(1, 10));
  Controller ctrl;
  assert(!validate(ctrl.config()));
  assert(ctrl.find("R01-B01") == 0 && ctrl.find("missing") == -1);
  Pixel frame[75]; ctrl.render(0, 0, frame);
  for (auto p : frame) assert(!p.r && !p.g && !p.b);
  Command cmd; cmd.on = true; cmd.brightness = 255;
  assert(ctrl.set(0, cmd, 100));
  assert(ctrl.state(0).brightness == 128);
  ctrl.render(0, 100, frame);
  assert(frame[0].g == 128 && frame[2].g == 128 && frame[3].g == 0);
  assert(ctrl.remaining(0, 59999 + 100) == 1);
  ctrl.update(60100); assert(!ctrl.state(0).on);
  cmd.blink = true; ctrl.set(0, cmd, 0);
  ctrl.render(0, 499, frame); assert(frame[0].g);
  ctrl.render(0, 500, frame); assert(!frame[0].g);
  ctrl.render(0, 1000, frame); assert(frame[0].g);
  const uint32_t wrapStart = std::numeric_limits<uint32_t>::max() - 499;
  cmd.durationMs = 1000; ctrl.set(0, cmd, wrapStart);
  assert(ctrl.remaining(0, 0) == 500); ctrl.update(500); assert(!ctrl.state(0).on);
  cmd.blink = false; cmd.durationMs = 1000; ctrl.set(0, cmd, 0);
  cmd.color[0] = 255; ctrl.set(0, cmd, 900);
  assert(ctrl.remaining(0, 1000) == 900 && ctrl.state(0).color[0] == 255);
  cmd.durationMs = 999; assert(!ctrl.set(0, cmd, 0));
  cmd.durationMs = 3600001; assert(!ctrl.set(0, cmd, 0));
  assert(!ctrl.set(100, cmd, 0));
  Config full{}; full.boxCount = 100;
  for (size_t o = 0; o < 4; ++o) {
    full.ledCounts[o] = 75;
    for (size_t b = 0; b < 25; ++b) {
      auto& box = full.boxes[o*25+b];
      snprintf(box.id, sizeof(box.id), "R%02u-B%02u", unsigned(o+1), unsigned(b+1));
      box.output = o; box.start = b*3; box.count = 3;
    }
  }
  assert(!validate(full));
  FakeStore store; store.succeed = false;
  assert(!commitConfig(ctrl, full, store));
  assert(ctrl.config().boxCount == 1 && ctrl.state(0).on);
  store.succeed = true; assert(commitConfig(ctrl, full, store));
  assert(ctrl.config().boxCount == 100 && !ctrl.state(0).on);
  cmd.durationMs = 60000; cmd.color[0] = 0;
  for (size_t i = 0; i < 100; ++i) assert(ctrl.set(i, cmd, 0));
  for (size_t o = 0; o < 4; ++o) {
    ctrl.render(o, 1, frame); for (auto p : frame) assert(p.g == 128);
  }
  assert(ctrl.find("R04-B25") == 99);
  Config bad = full;
  strcpy(bad.boxes[1].id, bad.boxes[0].id); assert(validate(bad));
  bad = full; bad.boxes[1].start = 2; assert(validate(bad));
  bad = full; bad.boxes[99].count = 4; assert(validate(bad));
  bad = full; bad.ledCounts[3] = 0; assert(validate(bad));
  bad = full; bad.ledCounts[0] = 76; assert(validate(bad));
  bad = full; bad.boxes[0].output = 4; assert(validate(bad));
  bad = full; strcpy(bad.boxes[0].id, "bad/id"); assert(validate(bad));
  bad = full; bad.boxCount = 101; assert(validate(bad));
  const unsigned writes = store.writes;
  assert(!commitConfig(ctrl, bad, store) && store.writes == writes);
  ctrl.allOff(); ctrl.render(3, 1, frame); for (auto p : frame) assert(!p.g);
  // Runtime commands and updates never call storage.
  ctrl.set(0, cmd, 0); ctrl.update(60000); assert(store.writes == writes);
  std::cout << "Domain tests passed: capacity, mapping, render, timers, rollover, transactions\n";
}
