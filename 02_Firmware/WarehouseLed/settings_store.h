#pragma once
#include <Arduino.h>
#include <Preferences.h>
#include "domain.h"
#include "profile_store.h"
class SettingsStore {
  Preferences prefs_;
 public:
  bool begin(Warehouse::Config& config);
  bool save(const Warehouse::Config& config, const String& ssid, const String& pass);
  bool provision();
  bool setApPassword(const String& password);
  bool migrateLab(const Warehouse::Config& config);
  uint32_t revision() const { return revision_; }
  Profiles::Repository<Preferences> profiles{prefs_};
  void printCredentials() const;
  bool ready() const { return token_.length() == 64 && apPass_.length() >= 8 && apPass_.length() <= 63 && otaPass_.length() >= 16; }
  const String& token() const { return token_; }
  const String& apPass() const { return apPass_; }
  const String& otaPass() const { return otaPass_; }
  const String& ssid() const { return ssid_; }
  const String& pass() const { return pass_; }
  const String& loadError() const { return loadError_; }
 private:
  bool open_ = false;
  uint32_t revision_ = 0;
  String token_, apPass_, otaPass_, ssid_, pass_, loadError_;
};
