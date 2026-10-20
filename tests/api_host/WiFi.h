#pragma once
#include "Arduino.h"
constexpr int WL_CONNECTED=3, WIFI_AP_STA=3;
struct AddressStub { String toString() { return "192.0.2.1"; } };
struct WiFiStub {
  bool connected = true;
  int status() { return connected ? WL_CONNECTED : 0; }
  AddressStub localIP() { return {}; }
  AddressStub softAPIP() { return {}; }
  void persistent(bool) {}
  void mode(int) {}
  void setSleep(bool) {}
  void begin(const char*, const char*) {}
  bool softAP(const char*, const char*) { return true; }
  void disconnect() {}
  void softAPdisconnect(bool) {}
};
inline WiFiStub WiFi;
