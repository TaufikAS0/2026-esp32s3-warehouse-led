#include "settings_store.h"
#include "codec.h"
#include "config.h"
#include <esp_system.h>
#include <bootloader_random.h>
namespace {
String randomHex(size_t bytes) {
  String s; s.reserve(bytes * 2);
  const char digits[] = "0123456789abcdef";
  for (size_t i = 0; i < bytes; ++i) {
    uint8_t b; esp_fill_random(&b, 1);
    s += digits[b >> 4]; s += digits[b & 15];
  }
  return s;
}
}
bool SettingsStore::begin(Warehouse::Config& config) {
  open_ = prefs_.begin("warehouse", false);
  if (!open_) { loadError_ = "NVS unavailable"; return false; }
  if (prefs_.isKey("credentials")) {
    JsonDocument auth;
    if (!deserializeJson(auth, prefs_.getString("credentials"))) {
      token_ = auth["token"].as<String>(); apPass_ = auth["ap"].as<String>(); otaPass_ = auth["ota"].as<String>();
    }
  }
  if (!prefs_.isKey("config")) return true;
  JsonDocument doc;
  String raw = prefs_.getString("config");
  if (raw.length() > Hardware::kMaxBodyBytes || deserializeJson(doc, raw) ||
      !Codec::decodeConfig(doc.as<JsonVariantConst>(), config, loadError_)) {
    Warehouse::setDefaults(config);
    if (loadError_.isEmpty()) loadError_ = "stored configuration is corrupt; defaults active";
    return false;
  }
  ssid_ = doc["wifi"]["ssid"].as<String>();
  pass_ = doc["wifi"]["password"].as<String>();
  return true;
}
bool SettingsStore::save(const Warehouse::Config& c, const String& ssid, const String& pass) {
  if (!open_ || Warehouse::validate(c)) return false;
  JsonDocument doc;
  Codec::encodeConfig(c, doc.to<JsonObject>());
  doc["wifi"]["ssid"] = ssid; doc["wifi"]["password"] = pass;
  if (doc.overflowed()) return false;
  String raw;
  serializeJson(doc, raw);
  if (raw.length() > Hardware::kMaxBodyBytes || prefs_.putString("config", raw) != raw.length()) return false;
  // One NVS key holds the entire mapping and network config, no partial-key transaction.
  ssid_ = ssid; pass_ = pass; loadError_ = "";
  return true;
}
bool SettingsStore::provision() {
  if (!open_ || ready()) return false;
  JsonDocument doc;
  // Provisioning runs before Wi-Fi/ADC startup. Supply non-RF entropy for credential generation.
  bootloader_random_enable();
  const String token = randomHex(32), ap = randomHex(12), ota = randomHex(16);
  bootloader_random_disable();
  doc["token"] = token; doc["ap"] = ap; doc["ota"] = ota;
  String raw; serializeJson(doc, raw);
  if (prefs_.putString("credentials", raw) != raw.length()) return false;
  token_ = token; apPass_ = ap; otaPass_ = ota;
  printCredentials();
  return true;
}
void SettingsStore::printCredentials() const {
  if (!ready()) { Serial.println("Not provisioned. Type provision over USB."); return; }
  // Only explicit physical USB commands disclose credentials. Never log them on boot or HTTP.
  Serial.printf("API_TOKEN=%s\nAP_PASSWORD=%s\nOTA_PASSWORD=%s\n", token_.c_str(), apPass_.c_str(), otaPass_.c_str());
}
