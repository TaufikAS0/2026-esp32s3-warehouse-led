#pragma once
#include <stdint.h>
#include <stddef.h>
namespace Hardware {
constexpr uint8_t kLedPins[4] = {4, 5, 6, 7};
constexpr uint32_t kSerialBaud = 115200;
constexpr uint32_t kWifiTimeoutMs = 15000;
constexpr uint32_t kWifiRetryMs = 30000;
constexpr uint32_t kOtaWindowMs = 600000;
constexpr size_t kMaxBodyBytes = 32768;
}
