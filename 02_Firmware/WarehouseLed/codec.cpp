#include "codec.h"
namespace {
bool number(JsonVariantConst v, unsigned low, unsigned high) {
  return v.is<unsigned>() && v.as<unsigned>() >= low && v.as<unsigned>() <= high;
}
bool text(JsonVariantConst v, size_t max) {
  if (!v.is<const char*>()) return false;
  JsonString s = v.as<JsonString>();
  return s.size() <= max && strlen(s.c_str()) == s.size();
}
}
bool Codec::decodeConfig(JsonVariantConst root, Warehouse::Config& out, String& error) {
  auto fail = [&](const char* e) { error = e; return false; };
  if (!root.is<JsonObjectConst>() || !number(root["schema"], 1, 1)) return fail("unsupported config schema");
  auto outputs = root["outputs"].as<JsonArrayConst>();
  auto boxes = root["boxes"].as<JsonArrayConst>();
  if (outputs.size() != 4 || boxes.size() < 1 || boxes.size() > 100 ||
      !number(root["brightness_limit"], 0, 255)) return fail("invalid config shape or limits");
  // Fill only validated entries; avoid an 8 KB Config temporary on the ESP32 loop stack.
  out.brightnessLimit = root["brightness_limit"].as<unsigned>();
  for (size_t i = 0; i < 4; ++i) {
    if (!number(outputs[i], 0, 75)) return fail("LED counts must be 0..75");
    out.ledCounts[i] = outputs[i].as<unsigned>();
  }
  out.boxCount = boxes.size();
  for (size_t i = 0; i < out.boxCount; ++i) {
    const auto b = boxes[i];
    if (!text(b["id"], 24) || !text(b["name"], 48) ||
        !number(b["output"], 1, 4) || !number(b["start_led"], 1, 75) || !number(b["led_count"], 1, 75))
      return fail("invalid box fields (ID <=24 bytes, name <=48 bytes)");
    auto& dest = out.boxes[i];
    strcpy(dest.id, b["id"].as<const char*>());
    strcpy(dest.name, b["name"].as<const char*>());
    dest.output = b["output"].as<unsigned>() - 1;
    dest.start = b["start_led"].as<unsigned>() - 1;
    dest.count = b["led_count"].as<unsigned>();
  }
  const char* e = Warehouse::validate(out);
  return e ? fail(e) : true;
}
void Codec::encodeConfig(const Warehouse::Config& c, JsonObject out) {
  out["schema"] = 1;
  out["brightness_limit"] = c.brightnessLimit;
  auto outputs = out["outputs"].to<JsonArray>();
  for (auto count : c.ledCounts) outputs.add(count);
  auto boxes = out["boxes"].to<JsonArray>();
  for (size_t i = 0; i < c.boxCount; ++i) {
    const auto& b = c.boxes[i];
    auto item = boxes.add<JsonObject>();
    item["id"] = b.id; item["name"] = b.name;
    item["output"] = b.output + 1; item["start_led"] = b.start + 1; item["led_count"] = b.count;
  }
}
bool Codec::decodeCommand(JsonVariantConst root, Warehouse::Command& out, String& error) {
  auto fail = [&](const char* e) { error = e; return false; };
  if (!root.is<JsonObjectConst>() || !root["on"].is<bool>()) return fail("on must be a boolean");
  out = Warehouse::Command{};
  out.on = root["on"].as<bool>();
  if (!root["color"].isUnbound()) {
    auto a = root["color"].as<JsonArrayConst>();
    if (a.size() != 3) return fail("color requires three integers");
    for (size_t i = 0; i < 3; ++i) {
      if (!number(a[i], 0, 255)) return fail("color channels must be 0..255");
      out.color[i] = a[i].as<unsigned>();
    }
  }
  if (!root["brightness"].isUnbound()) {
    if (!number(root["brightness"], 0, 255)) return fail("brightness must be 0..255");
    out.brightness = root["brightness"].as<unsigned>();
  }
  if (!root["mode"].isUnbound()) {
    if (!text(root["mode"], 6)) return fail("invalid mode");
    const String mode = root["mode"].as<const char*>();
    if (mode != "steady" && mode != "blink") return fail("mode must be steady or blink");
    out.blink = mode == "blink";
  }
  if (!root["duration_ms"].isUnbound()) {
    if (!number(root["duration_ms"], 1000, 3600000)) return fail("duration_ms must be 1000..3600000");
    out.durationMs = root["duration_ms"].as<unsigned>();
  }
  return true;
}
bool Codec::authenticated(const String& header, const String& token) {
  if (token.length() != 64 || header.length() != token.length() + 7) return false;
  uint8_t difference = 0;
  const char prefix[] = "Bearer ";
  for (size_t i = 0; i < 7; ++i) difference |= header[i] ^ prefix[i];
  for (size_t i = 0; i < token.length(); ++i) difference |= header[i + 7] ^ token[i];
  return difference == 0;
}
