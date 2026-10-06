#pragma once
#include <Arduino.h>
#include <Preferences.h>
#include "domain.h"
class SettingsStore {
 public:
  bool begin(Warehouse::Config& config);
  bool save(const Warehouse::Config& config, const String& ssid, const String& pass);
  bool provision();
  void printCredentials() const;
  bool ready() const { return token_.length() == 64 && apPass_.length() >= 16 && otaPass_.length() >= 16; }
  const String& token() const { return token_; }
  const String& apPass() const { return apPass_; }
  const String& otaPass() const { return otaPass_; }
  const String& ssid() const { return ssid_; }
  const String& pass() const { return pass_; }
  const String& loadError() const { return loadError_; }
 private:
  Preferences prefs_;
  bool open_ = false;
  String token_, apPass_, otaPass_, ssid_, pass_, loadError_;
};
