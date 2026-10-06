#include "codec.h"
#include <cassert>
#include <iostream>
bool command(const char* json) {
  JsonDocument doc; assert(!deserializeJson(doc, json));
  Warehouse::Command cmd; String error;
  return Codec::decodeCommand(doc.as<JsonVariantConst>(), cmd, error);
}
int main() {
  JsonDocument doc; Warehouse::Config c = Warehouse::defaults();
  Codec::encodeConfig(c, doc.to<JsonObject>());
  Warehouse::Config decoded; String e;
  assert(Codec::decodeConfig(doc.as<JsonVariantConst>(), decoded, e));
  assert(!strcmp(decoded.boxes[0].id, "R01-B01") && decoded.boxes[0].start == 0);
  doc["schema"] = true; assert(!Codec::decodeConfig(doc.as<JsonVariantConst>(), decoded, e));
  doc["schema"] = 1;
  for (const char* value : {"-1", "76", "1.5", "true", "null", "\"3\""}) {
    JsonDocument n; deserializeJson(n, value); doc["outputs"][0].set(n.as<JsonVariantConst>());
    assert(!Codec::decodeConfig(doc.as<JsonVariantConst>(), decoded, e));
  }
  doc["outputs"][0] = 3;
  doc["boxes"][0]["start_led"] = 0; assert(!Codec::decodeConfig(doc.as<JsonVariantConst>(), decoded, e));
  doc["boxes"][0]["start_led"] = 1;
  // Maximum-size labels/IDs exercise the actual wire format and storage payload budget.
  Warehouse::Config full{}; full.boxCount = 100;
  for (size_t o = 0; o < 4; ++o) {
    full.ledCounts[o] = 75;
    for (size_t b = 0; b < 25; ++b) {
      auto& box = full.boxes[o*25+b];
      snprintf(box.id, sizeof(box.id), "R%02u-B%02u-abcdefghijklmnop", unsigned(o+1), unsigned(b+1));
      memset(box.name, 'x', 48); box.name[48] = 0;
      box.output = o; box.start = b*3; box.count = 3;
    }
  }
  doc.clear(); Codec::encodeConfig(full, doc.to<JsonObject>());
  assert(Codec::decodeConfig(doc.as<JsonVariantConst>(), decoded, e));
  assert(decoded.boxCount == 100 && decoded.boxes[99].start == 72);
  assert(measureJson(doc) < 32768);
  assert(command("{\"on\":true}")); assert(command("{\"on\":false}"));
  assert(command("{\"on\":true,\"color\":[0,255,1],\"brightness\":0,\"mode\":\"blink\",\"duration_ms\":3600000}"));
  for (const char* raw : {"{}", "{\"on\":1}", "{\"on\":true,\"color\":null}",
    "{\"on\":true,\"color\":[0,255]}", "{\"on\":true,\"color\":[0,-1,0]}",
    "{\"on\":true,\"color\":[0,256,0]}", "{\"on\":true,\"color\":[0,1.5,0]}",
    "{\"on\":true,\"brightness\":true}", "{\"on\":true,\"mode\":\"random\"}",
    "{\"on\":true,\"duration_ms\":999}", "{\"on\":true,\"duration_ms\":3600001}",
    "{\"on\":true,\"duration_ms\":null}"}) assert(!command(raw));
  std::cout << "Codec tests passed: real JSON parsing, numeric bounds, config roundtrip\n";
}
