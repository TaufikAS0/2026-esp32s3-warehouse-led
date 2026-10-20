#include "settings_store.h"
#include "codec.h"
#include "config.h"
#include <generated_lab.h>
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
  if (!prefs_.isKey("config_blob") && !prefs_.isKey("config")) return true;
  JsonDocument doc;
  // Prefer the new blob only when present. A corrupt blob must not resurrect an old mapping.
  if (prefs_.isKey("config_blob")) {
    const size_t size = prefs_.getBytesLength("config_blob");
    auto raw = size && size <= Hardware::kMaxBodyBytes
        ? std::unique_ptr<uint8_t[]>(new(std::nothrow) uint8_t[size]) : nullptr;
    if (!raw || prefs_.getBytes("config_blob", raw.get(), size) != size ||
        deserializeJson(doc, raw.get(), size)) {
      Warehouse::setDefaults(config);
      loadError_ = "stored configuration blob unreadable; defaults active";
      return false;
    }
  } else {
    const String raw = prefs_.getString("config");
    if (raw.length() > Hardware::kMaxBodyBytes || deserializeJson(doc, raw)) {
      Warehouse::setDefaults(config);
      loadError_ = "stored configuration is corrupt; defaults active";
      return false;
    }
  }
  if (!Codec::decodeConfig(doc.as<JsonVariantConst>(), config, loadError_)) {
    Warehouse::setDefaults(config);
    if (loadError_.isEmpty()) loadError_ = "stored configuration is corrupt; defaults active";
    return false;
  }
  ssid_ = doc["wifi"]["ssid"].as<String>();
  pass_ = doc["wifi"]["password"].as<String>();
  revision_ = doc["revision"] | 0u;
  return true;
}
bool SettingsStore::save(const Warehouse::Config& c, const String& ssid, const String& pass) {
  if (!open_ || revision_ == UINT32_MAX || Warehouse::validate(c)) return false;
  JsonDocument doc;
  Codec::encodeConfig(c, doc.to<JsonObject>());
  doc["wifi"]["ssid"] = ssid; doc["wifi"]["password"] = pass;
  doc["revision"] = revision_ + 1;
  if (doc.overflowed()) return false;
  String raw;
  const size_t expected = measureJson(doc);
  if (expected > Hardware::kMaxBodyBytes) return false;
  serializeJson(doc, raw);
  if (raw.length() != expected) return false; // String OOM must never persist truncated JSON.
  // NVS strings are limited to 4000 bytes including NUL. Blobs span multiple pages.
  // A distinct key preserves the legacy record if the first blob write fails.
  if (prefs_.putBytes("config_blob", raw.c_str(), raw.length()) != raw.length()) return false;
  // One NVS key holds the entire mapping and network config, no partial-key transaction.
  ssid_ = ssid; pass_ = pass; loadError_ = ""; ++revision_;
  return true;
}
bool SettingsStore::migrateLab(const Warehouse::Config& config) {
  // Do not overwrite a corrupt original config with fallback defaults.
  if (!open_ || !loadError_.isEmpty()) return false;
  if ((ssid_ != Lab::kSsid || pass_ != Lab::kPassword) && !save(config, Lab::kSsid, Lab::kPassword)) {
    loadError_ = "lab network migration failed; previous config retained"; return false;
  }
  if (ready() && apPass_ != Hardware::kDefaultApPassword && !setApPassword(Hardware::kDefaultApPassword)) {
    loadError_ = "AP migration failed; previous credentials retained"; return false;
  }
  return true;
}
bool SettingsStore::provision() {
  if (!open_ || ready()) return false;
  JsonDocument doc;
  // Provisioning runs before Wi-Fi/ADC startup. Supply non-RF entropy for credential generation.
  bootloader_random_enable();
  const String token = randomHex(32), ap = Hardware::kDefaultApPassword, ota = randomHex(16);
  bootloader_random_disable();
  if (token.length() != 64 || ap.length() != 8 || ota.length() != 32) return false;
  doc["token"] = token; doc["ap"] = ap; doc["ota"] = ota;
  String raw; serializeJson(doc, raw);
  if (doc.overflowed() || raw.length() != measureJson(doc)) return false;
  if (prefs_.putString("credentials", raw) != raw.length()) return false;
  token_ = token; apPass_ = ap; otaPass_ = ota;
  printCredentials();
  return true;
}
bool SettingsStore::setApPassword(const String& password) {
  if (!open_ || !ready() || password.length() < 8 || password.length() > 63) return false;
  for (size_t i = 0; i < password.length(); ++i) {
    if (password[i] < 32 || password[i] > 126) return false;
  }
  if (password == apPass_) return true;
  JsonDocument doc;
  doc["token"] = token_; doc["ap"] = password; doc["ota"] = otaPass_;
  String raw; serializeJson(doc, raw);
  if (doc.overflowed() || raw.length() != measureJson(doc)) return false;
  if (prefs_.putString("credentials", raw) != raw.length()) return false;
  apPass_ = password;
  return true;
}
void SettingsStore::printCredentials() const {
  if (!ready()) { Serial.println("Not provisioned. Type provision over USB."); return; }
  // Only explicit physical USB commands disclose credentials. Never log them on boot or HTTP.
  Serial.printf("API_TOKEN=%s\nAP_PASSWORD=%s\nOTA_PASSWORD=%s\n", token_.c_str(), apPass_.c_str(), otaPass_.c_str());
}
