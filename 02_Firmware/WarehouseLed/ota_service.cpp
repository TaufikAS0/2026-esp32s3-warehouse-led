#include "ota_service.h"
#include "config.h"
#include <ArduinoOTA.h>
void OtaService::begin(SettingsStore& store, WifiService& wifi) { store_ = &store; wifi_ = &wifi; }
bool OtaService::enable(uint32_t now) {
  if (!store_->ready() || !wifi_->sta()) return false;
  if (!active_) {
    ArduinoOTA.setHostname(wifi_->apName().c_str());
    ArduinoOTA.setPassword(store_->otaPass().c_str());
    // Tolerate packet gaps during maintenance on a congested warehouse WLAN.
    ArduinoOTA.setTimeout(Hardware::kOtaTransferTimeoutMs);
    ArduinoOTA.onStart([] { Serial.println("OTA transfer started; maintenance in progress."); });
    ArduinoOTA.onEnd([] { Serial.println("OTA transfer complete; rebooting."); });
    ArduinoOTA.onError([](ota_error_t error) { Serial.printf("OTA error=%u; inspect network/upload log.\n", unsigned(error)); });
    ArduinoOTA.begin(); active_ = true;
  }
  started_ = now;
  return true;
}
void OtaService::disable() { if (active_) ArduinoOTA.end(); active_ = false; }
uint32_t OtaService::remaining(uint32_t now) const {
  const uint32_t elapsed = now - started_;
  return active_ && elapsed < Hardware::kOtaWindowMs ? Hardware::kOtaWindowMs - elapsed : 0;
}
void OtaService::update(uint32_t now) {
  if (active_ && (!wifi_->sta() || !remaining(now))) disable();
  if (active_) ArduinoOTA.handle();
}
