#pragma once
#include "web_service.h"
class App {
 public:
  App() : web_(controller_, store_, leds_, wifi_, ota_) {}
  void begin();
  void update();
 private:
  Warehouse::Controller controller_;
  SettingsStore store_;
  LedDriver leds_;
  WifiService wifi_;
  OtaService ota_;
  WebService web_;
  char serialLine_[76]{};
  size_t serialLength_ = 0;
  bool serialOverflow_ = false;
  void serialUpdate();
};
