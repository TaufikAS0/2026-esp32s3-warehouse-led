#pragma once
#include <WiFi.h>
#include "settings_store.h"
class WifiService {
 public:
  void begin(SettingsStore& store);
  void update(uint32_t now);
  void scheduleRestart(uint32_t now) { restart_ = true; restartAt_ = now; }
  bool sta() const { return WiFi.status() == WL_CONNECTED; }
  bool ap() const { return ap_; }
  String ip() const { return sta() ? WiFi.localIP().toString() : (ap_ ? WiFi.softAPIP().toString() : ""); }
  const String& apName() const { return name_; }
 private:
  SettingsStore* store_ = nullptr;
  String name_;
  bool ap_ = false, started_ = false, restart_ = false;
  uint32_t attemptAt_ = 0, restartAt_ = 0;
  void start(uint32_t now);
  void startAp();
};
