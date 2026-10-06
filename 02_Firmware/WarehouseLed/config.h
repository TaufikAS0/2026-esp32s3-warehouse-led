#pragma once
#include <stdint.h>
#include <stddef.h>
namespace Hardware {
// User-approved shared AP default for internal setup; not an API/OTA credential.
constexpr const char* kDefaultApPassword = "12345678";
constexpr uint8_t kLedPins[4] = {4, 5, 6, 7};
constexpr uint32_t kSerialBaud = 115200;
constexpr uint32_t kWifiTimeoutMs = 15000;
constexpr uint32_t kWifiRetryMs = 30000;
constexpr uint32_t kOtaWindowMs = 600000;
constexpr uint32_t kOtaTransferTimeoutMs = 10000;
constexpr size_t kMaxBodyBytes = 32768;
}
