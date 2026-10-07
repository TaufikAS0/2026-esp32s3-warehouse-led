#include "web_service.h"
#include "config.h"
#include "firmware_version.h"
#include "web_ui.h"
#include <memory>
void WebService::error(int code, const String& message) {
  JsonDocument doc; doc["error"] = message; send(code, doc);
}
void WebService::send(int code, const JsonDocument& doc) {
  if (doc.overflowed()) { server_.send(503, "application/json", "{\"error\":\"JSON allocation failed\"}"); return; }
  String raw; serializeJson(doc, raw);
  server_.sendHeader("Cache-Control", "no-store");
  server_.send(code, "application/json", raw);
}
bool WebService::body(JsonDocument& doc) {
  if (!server_.header("Content-Type").startsWith("application/json")) { error(415, "application/json required"); return false; }
  const String& raw = server_.arg("plain");
  if (raw.length() > Hardware::kMaxBodyBytes) { error(413, "request exceeds 32768 bytes"); return false; }
  if (deserializeJson(doc, raw) || !doc.is<JsonObject>()) { error(400, "invalid JSON object"); return false; }
  return true;
}
void WebService::state(JsonObject out, size_t i, uint32_t now) {
  const auto& s = controller_.state(i);
  out["id"] = controller_.config().boxes[i].id;
  out["on"] = s.on && controller_.remaining(i, now) > 0;
  out["remaining_ms"] = controller_.remaining(i, now);
  out["brightness"] = s.brightness;
  out["mode"] = s.blink ? "blink" : "steady";
  auto color = out["color"].to<JsonArray>();
  for (auto channel : s.color) color.add(channel);
  const auto& program = leds_.program();
  const bool automatic = program.boxLit(i, controller_, now) && !leds_.hardwareTests().remaining(controller_.config().boxes[i].output, now);
  out["program_on"] = automatic;
  const auto value = program.color(i, controller_.config(), now);
  auto automaticColor = out["program_color"].to<JsonArray>();
  automaticColor.add(value.r); automaticColor.add(value.g); automaticColor.add(value.b);
}
void WebService::status() {
  const uint32_t now = millis(); controller_.update(now);
  JsonDocument doc;
  doc["version"] = Firmware::kVersion; doc["uptime_ms"] = now;
  doc["sta_connected"] = wifi_.sta(); doc["ap_active"] = wifi_.ap(); doc["ip"] = wifi_.ip();
  doc["ap_ssid"] = wifi_.apName(); doc["led_driver_ready"] = leds_.ready();
  doc["storage_error"] = store_.loadError(); doc["ota_remaining_ms"] = ota_.remaining(now);
  doc["brightness_limit"] = controller_.config().brightnessLimit;
  auto program = doc["program"].to<JsonObject>();
  program["on"] = leds_.program().active(now); program["idle"] = leds_.program().idle();
  program["pattern"] = Warehouse::patternName(leds_.program().pattern());
  program["brightness"] = leds_.program().brightness(); program["step_ms"] = leds_.program().step();
  program["remaining_ms"] = leds_.program().remaining(now);
  auto outputs = doc["outputs"].to<JsonArray>();
  for (auto count : controller_.config().ledCounts) outputs.add(count);
  auto tests = doc["hardware_tests"].to<JsonArray>();
  for (size_t o = 0; o < Warehouse::kOutputs; ++o) {
    const auto& s = leds_.hardwareTests().state(o);
    auto test = tests.add<JsonObject>();
    test["output"] = o + 1; test["remaining_ms"] = leds_.hardwareTests().remaining(o, now);
    test["on"] = leds_.hardwareTests().remaining(o, now) > 0;
    test["brightness"] = s.brightness; test["mode"] = s.blink ? "blink" : "steady";
    auto color = test["color"].to<JsonArray>(); for (auto channel : s.color) color.add(channel);
  }
  auto list = doc["boxes"].to<JsonArray>();
  for (size_t i = 0; i < controller_.config().boxCount; ++i) state(list.add<JsonObject>(), i, now);
  send(200, doc);
}
void WebService::boxes() {
  JsonDocument doc; Codec::encodeConfig(controller_.config(), doc.to<JsonObject>()); send(200, doc);
}
void WebService::getConfig() {
  JsonDocument doc; Codec::encodeConfig(controller_.config(), doc.to<JsonObject>());
  doc["wifi"]["ssid"] = store_.ssid(); doc["wifi"]["password_configured"] = !store_.pass().isEmpty();
  send(200, doc);
}
void WebService::putConfig() {
  JsonDocument doc; if (!body(doc)) return;
  auto candidate = std::unique_ptr<Warehouse::Config>(new(std::nothrow) Warehouse::Config);
  if (!candidate) { error(503, "configuration allocation failed"); return; }
  String e;
  if (!Codec::decodeConfig(doc.as<JsonVariantConst>(), *candidate, e)) { error(400, e); return; }
  String ssid = store_.ssid(), pass = store_.pass();
  if (!doc["wifi"].isUnbound()) {
    const auto wifi = doc["wifi"];
    if (!wifi.is<JsonObject>() || !wifi["ssid"].is<const char*>()) { error(400, "wifi.ssid required"); return; }
    const JsonString text = wifi["ssid"].as<JsonString>();
    if (text.size() > 32 || text.size() != strlen(text.c_str())) { error(400, "invalid SSID"); return; }
    ssid = text.c_str();
    if (!wifi["password"].isUnbound()) {
      if (!wifi["password"].is<const char*>()) { error(400, "invalid Wi-Fi password"); return; }
      const JsonString secret = wifi["password"].as<JsonString>();
      if (secret.size() != strlen(secret.c_str()) || (secret.size() && (secret.size() < 8 || secret.size() > 63))) {
        error(400, "Wi-Fi password must be empty or 8..63 bytes"); return;
      }
      pass = secret.c_str();
    } else if (ssid != store_.ssid() && !ssid.isEmpty()) {
      error(400, "password required when changing SSID (empty for open network)"); return;
    }
    if (ssid.isEmpty()) pass = "";
  }
  const bool networkChanged = ssid != store_.ssid() || pass != store_.pass();
  struct Transaction {
    SettingsStore& store; const String& ssid; const String& pass;
    bool save(const Warehouse::Config& c) { return store.save(c, ssid, pass); }
  } transaction{store_, ssid, pass};
  if (!Warehouse::commitConfig(controller_, *candidate, transaction)) { error(503, "configuration write failed; previous config active"); return; }
  leds_.off();
  if (networkChanged) { ota_.disable(); wifi_.scheduleRestart(millis()); }
  getConfig();
}
void WebService::routeUnknown() {
  const String outputPrefix = "/api/v1/outputs/", testSuffix = "/test";
  const String path = server_.uri();
  if (path.startsWith(outputPrefix) && path.endsWith(testSuffix)) {
    if (server_.method() != HTTP_PUT) { error(405, "PUT required"); return; }
    const String id = path.substring(outputPrefix.length(), path.length() - testSuffix.length());
    if (id.length() != 1 || id[0] < '1' || id[0] > '4') { error(404, "output not found"); return; }
    const size_t o = id[0] - '1';
    JsonDocument doc; if (!body(doc)) return;
    Warehouse::Command command; command.durationMs = 3000; String e;
    // Codec supplies the standard 60-second default; hardware tests use 3 seconds.
    if (!Codec::decodeCommand(doc.as<JsonVariantConst>(), command, e)) { error(400, e); return; }
    if (doc["duration_ms"].isUnbound()) command.durationMs = 3000;
    if (command.durationMs > 30000) { error(400, "hardware test maximum 30000 ms"); return; }
    if (command.on && !controller_.config().ledCounts[o]) { error(400, "output has no installed LEDs"); return; }
    if (!leds_.ready()) { error(503, "LED driver unavailable"); return; }
    const uint32_t now = millis(); Warehouse::Command off;
    for (size_t i = 0; i < controller_.config().boxCount; ++i)
      if (controller_.config().boxes[i].output == o) controller_.set(i, off, now);
    leds_.hardwareTests().set(o, command, controller_.config(), now);
    leds_.update(controller_, now);
    JsonDocument result; result["output"] = o + 1;
    result["on"] = leds_.hardwareTests().remaining(o, now) > 0;
    result["remaining_ms"] = leds_.hardwareTests().remaining(o, now);
    result["brightness"] = leds_.hardwareTests().state(o).brightness;
    result["led_count"] = controller_.config().ledCounts[o]; send(200, result); return;
  }
  const String prefix = "/api/v1/boxes/", suffix = "/state";
  const String uri = server_.uri();
  if (uri.startsWith(prefix) && uri.endsWith(suffix)) {
    if (server_.method() != HTTP_PUT) { error(405, "PUT required"); return; }
    const String id = uri.substring(prefix.length(), uri.length() - suffix.length());
    const int i = controller_.find(id.c_str());
    if (i < 0) { error(404, "box ID not found"); return; }
    JsonDocument doc; if (!body(doc)) return;
    Warehouse::Command command; String e;
    if (!Codec::decodeCommand(doc.as<JsonVariantConst>(), command, e)) { error(400, e); return; }
    if (!leds_.ready()) { error(503, "LED driver unavailable"); return; }
    const uint32_t now = millis();
    if (command.on && doc["color"].isUnbound())
      memcpy(command.color, controller_.config().boxes[i].color, sizeof(command.color));
    leds_.hardwareTests().cancel(controller_.config().boxes[i].output);
    controller_.set(i, command, now); controller_.update(now); leds_.update(controller_, now);
    JsonDocument response; state(response.to<JsonObject>(), i, now); send(200, response);
    return;
  }
  if (uri == "/api/v1/status" || uri == "/api/v1/boxes" || uri == "/api/v1/config" ||
      uri == "/api/v1/all-off" || uri == "/api/v1/ota" || uri == "/api/v1/program") error(405, "unsupported method");
  else error(404, "endpoint not found");
}
void WebService::begin() {
  const char* headers[] = {"Content-Type"}; server_.collectHeaders(headers, 1);
  server_.on("/", HTTP_GET, [this] {
    server_.sendHeader("Cache-Control", "no-store");
    server_.sendHeader("X-Content-Type-Options", "nosniff");
    server_.sendHeader("Content-Security-Policy", "default-src 'self'; script-src 'unsafe-inline'; style-src 'unsafe-inline'; connect-src 'self'; frame-ancestors 'none'");
    server_.send_P(200, "text/html; charset=utf-8", kWebUi);
  });
  server_.on("/api/v1/status", HTTP_GET, [this] { status(); });
  server_.on("/api/v1/boxes", HTTP_GET, [this] { boxes(); });
  server_.on("/api/v1/config", HTTP_GET, [this] { getConfig(); });
  server_.on("/api/v1/config", HTTP_PUT, [this] { putConfig(); });
  server_.on("/api/v1/program", HTTP_PUT, [this] {
    JsonDocument doc; if (!body(doc)) return;
    if (!doc["on"].is<bool>()) { error(400, "on must be boolean"); return; }
    if (!doc["on"].as<bool>()) { leds_.program().stop(); leds_.update(controller_, millis()); status(); return; }
    const auto bounded = [&](const char* key, unsigned low, unsigned high) {
      return doc[key].isUnbound() || (doc[key].is<unsigned>() && doc[key].as<unsigned>() >= low && doc[key].as<unsigned>() <= high);
    };
    if ((!doc["idle"].isUnbound() && !doc["idle"].is<bool>()) ||
        !bounded("brightness", 0, 255) || !bounded("step_ms", 100, 5000) || !bounded("duration_ms", 1000, 3600000)) {
      error(400, "invalid program idle/brightness/step_ms/duration_ms"); return;
    }
    if (!doc["pattern"].is<const char*>()) { error(400, "pattern required"); return; }
    const String name = doc["pattern"].as<const char*>(); Warehouse::Pattern pattern;
    if (name == "running") pattern = Warehouse::Pattern::Running;
    else if (name == "blink") pattern = Warehouse::Pattern::Blink;
    else if (name == "rainbow") pattern = Warehouse::Pattern::Rainbow;
    else if (name == "breathe") pattern = Warehouse::Pattern::Breathe;
    else { error(400, "unknown program pattern"); return; }
    if (!leds_.ready()) { error(503, "LED driver unavailable"); return; }
    leds_.hardwareTests().clear();
    leds_.program().start(pattern, doc["idle"] | false, doc["brightness"] | 32u,
                          doc["step_ms"] | 500u, doc["duration_ms"] | 30000u, controller_.config(), millis());
    leds_.update(controller_, millis()); status();
  });
  server_.on("/api/v1/all-off", HTTP_POST, [this] {
      controller_.allOff(); leds_.off(); JsonDocument doc; doc["ok"] = true; send(200, doc);
  });
  server_.on("/api/v1/ota", HTTP_POST, [this] {
      JsonDocument doc; if (!body(doc)) return;
    if (!doc["enabled"].is<bool>()) { error(400, "enabled must be boolean"); return; }
    if (doc["enabled"].as<bool>()) {
      if (!wifi_.sta()) { error(409, "OTA requires STA connection"); return; }
      controller_.allOff(); leds_.off();
      if (!ota_.enable(millis())) { error(503, "OTA unavailable"); return; }
    } else ota_.disable();
    JsonDocument result; result["remaining_ms"] = ota_.remaining(millis()); send(200, result);
  });
  server_.onNotFound([this] { routeUnknown(); });
  // Register routes now; defer sockets until Wi-Fi has initialized the network stack.
}
void WebService::update() {
  if (!wifi_.sta() && !wifi_.ap()) {
    if (listening_) { server_.close(); listening_ = false; }
    return;
  }
  if (!listening_) { server_.begin(); listening_ = true; }
  server_.handleClient();
}
