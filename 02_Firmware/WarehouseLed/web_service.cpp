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
bool WebService::authorize() {
  if (Codec::authenticated(server_.header("Authorization"), store_.token()) && store_.ready()) return true;
  error(401, "invalid token or device not provisioned"); return false;
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
}
void WebService::status() {
  const uint32_t now = millis(); controller_.update(now);
  JsonDocument doc;
  doc["version"] = Firmware::kVersion; doc["uptime_ms"] = now;
  doc["sta_connected"] = wifi_.sta(); doc["ap_active"] = wifi_.ap(); doc["ip"] = wifi_.ip();
  doc["ap_ssid"] = wifi_.apName(); doc["led_driver_ready"] = leds_.ready();
  doc["storage_error"] = store_.loadError(); doc["ota_remaining_ms"] = ota_.remaining(now);
  doc["brightness_limit"] = controller_.config().brightnessLimit;
  auto outputs = doc["outputs"].to<JsonArray>();
  for (auto count : controller_.config().ledCounts) outputs.add(count);
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
  if (!authorize()) return;
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
    controller_.set(i, command, now); controller_.update(now); leds_.update(controller_, now);
    JsonDocument response; state(response.to<JsonObject>(), i, now); send(200, response);
    return;
  }
  if (uri == "/api/v1/status" || uri == "/api/v1/boxes" || uri == "/api/v1/config" ||
      uri == "/api/v1/all-off" || uri == "/api/v1/ota") error(405, "unsupported method");
  else error(404, "endpoint not found");
}
void WebService::begin() {
  const char* headers[] = {"Authorization", "Content-Type"}; server_.collectHeaders(headers, 2);
  server_.on("/", HTTP_GET, [this] {
    server_.sendHeader("Cache-Control", "no-store");
    server_.sendHeader("X-Content-Type-Options", "nosniff");
    server_.sendHeader("Content-Security-Policy", "default-src 'self'; script-src 'unsafe-inline'; style-src 'unsafe-inline'; connect-src 'self'; frame-ancestors 'none'");
    server_.send_P(200, "text/html; charset=utf-8", kWebUi);
  });
  server_.on("/api/v1/status", HTTP_GET, [this] { if (authorize()) status(); });
  server_.on("/api/v1/boxes", HTTP_GET, [this] { if (authorize()) boxes(); });
  server_.on("/api/v1/config", HTTP_GET, [this] { if (authorize()) getConfig(); });
  server_.on("/api/v1/config", HTTP_PUT, [this] { if (authorize()) putConfig(); });
  server_.on("/api/v1/all-off", HTTP_POST, [this] {
    if (!authorize()) return;
    controller_.allOff(); leds_.off(); JsonDocument doc; doc["ok"] = true; send(200, doc);
  });
  server_.on("/api/v1/ota", HTTP_POST, [this] {
    if (!authorize()) return;
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
