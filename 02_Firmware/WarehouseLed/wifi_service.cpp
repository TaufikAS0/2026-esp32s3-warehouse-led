#include "wifi_service.h"
#include "config.h"
void WifiService::begin(SettingsStore& store) {
  store_ = &store;
  char suffix[7]; snprintf(suffix, sizeof(suffix), "%06llX", ESP.getEfuseMac() & 0xFFFFFF);
  name_ = String("Warehouse-") + suffix;
  WiFi.persistent(false); // Network writes are explicitly owned by SettingsStore.
}
void WifiService::start(uint32_t now) {
  WiFi.mode(WIFI_AP_STA);
  WiFi.setSleep(false); // Mains-powered controller: prioritize interactive HTTP/OTA latency.
  started_ = true; attemptAt_ = now;
  if (store_->ssid().isEmpty()) startAp();
  else WiFi.begin(store_->ssid().c_str(), store_->pass().c_str());
}
void WifiService::startAp() {
  if (!ap_) ap_ = WiFi.softAP(name_.c_str(), store_->apPass().c_str());
}
void WifiService::update(uint32_t now) {
  if (!store_->ready()) return;
  if (restart_ && now - restartAt_ >= 1000) {
    WiFi.disconnect(); WiFi.softAPdisconnect(true);
    ap_ = false; started_ = false; restart_ = false;
  }
  if (!started_) start(now);
  if (sta()) {
    if (ap_) { WiFi.softAPdisconnect(false); ap_ = false; }
    attemptAt_ = now;
    return;
  }
  if (store_->ssid().isEmpty() || now - attemptAt_ >= Hardware::kWifiTimeoutMs) startAp();
  if (!store_->ssid().isEmpty() && now - attemptAt_ >= Hardware::kWifiRetryMs) {
    WiFi.begin(store_->ssid().c_str(), store_->pass().c_str()); attemptAt_ = now;
  }
}
