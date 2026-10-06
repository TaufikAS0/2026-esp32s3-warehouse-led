#include "app.h"
#include "config.h"
#include "firmware_version.h"
#include <memory>
void App::begin() {
  Serial.begin(Hardware::kSerialBaud);
  Serial.printf("\nWarehouse LED %s\n", Firmware::kVersion);
  auto config = std::unique_ptr<Warehouse::Config>(new(std::nothrow) Warehouse::Config);
  if (config) {
    Warehouse::setDefaults(*config);
    store_.begin(*config); controller_.configure(*config);
  } else Serial.println("Configuration allocation failed; USB provisioning unavailable");
  if (!leds_.begin()) Serial.println("LED buffer allocation failed");
  if (!store_.loadError().isEmpty()) Serial.println(store_.loadError());
  wifi_.begin(store_); ota_.begin(store_, wifi_); web_.begin();
  Serial.println("USB commands: provision (first setup), credentials (explicit read), info");
  if (!store_.ready()) Serial.println("Network locked until USB provision completes.");
}
void App::serialUpdate() {
  // Bound serial work so a noisy USB sender cannot starve timers or web handling.
  for (unsigned budget = 0; budget < 64 && Serial.available(); ++budget) {
    const char c = Serial.read();
    if (c == '\r') continue;
    if (c == '\n') {
      serialLine_[serialLength_] = 0;
      if (serialOverflow_) Serial.println("Command too long");
      else if (!strcmp(serialLine_, "provision")) {
        if (!store_.provision()) Serial.println("Provision refused (already provisioned or NVS write failed).");
      } else if (!strcmp(serialLine_, "credentials")) store_.printCredentials();
      else if (!strcmp(serialLine_, "info"))
        Serial.printf("%s IP=%s STA=%d AP=%d\n", Firmware::kVersion, wifi_.ip().c_str(), wifi_.sta(), wifi_.ap());
      else if (serialLength_) Serial.println("Unknown USB command");
      serialLength_ = 0; serialOverflow_ = false;
    } else if (serialLength_ < sizeof(serialLine_) - 1) serialLine_[serialLength_++] = c;
    else serialOverflow_ = true;
  }
}
void App::update() {
  serialUpdate();
  const uint32_t now = millis();
  controller_.update(now); leds_.update(controller_, now);
  wifi_.update(now); ota_.update(now); web_.update();
}
