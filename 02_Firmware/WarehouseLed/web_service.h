#pragma once
#include <WebServer.h>
#include "led_driver.h"
#include "ota_service.h"
#include "codec.h"
class WebService {
 public:
  WebService(Warehouse::Controller& c, SettingsStore& s, LedDriver& l, WifiService& w, OtaService& o)
    : controller_(c), store_(s), leds_(l), wifi_(w), ota_(o) {}
  void begin();
  void update();
 private:
  WebServer server_{80};
  bool listening_ = false;
  Warehouse::Controller& controller_;
  SettingsStore& store_;
  LedDriver& leds_;
  WifiService& wifi_;
  OtaService& ota_;
  bool body(JsonDocument& doc);
  void error(int code, const String& message);
  void send(int code, const JsonDocument& doc);
  void status();
  void boxes();
  void getConfig();
  void putConfig();
  void state(JsonObject out, size_t i, uint32_t now);
  void routeUnknown();
};
