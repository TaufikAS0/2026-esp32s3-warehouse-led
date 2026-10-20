#pragma once
#include "wifi_service.h"
class OtaService {
 public:
  void begin(SettingsStore& store, WifiService& wifi);
  bool enable(uint32_t now);
  void disable();
  void update(uint32_t now);
  bool active() const { return active_; }
  uint32_t remaining(uint32_t now) const;
 private:
  SettingsStore* store_ = nullptr;
  WifiService* wifi_ = nullptr;
  uint32_t started_ = 0;
  bool active_ = false;
};
