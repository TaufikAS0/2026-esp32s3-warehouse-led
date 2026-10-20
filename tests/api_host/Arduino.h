#pragma once
#include "../storage_host/Arduino.h"
#define PROGMEM
inline uint32_t testNow = 0;
inline uint32_t millis() { return testNow; }
struct ESPStub { uint64_t getEfuseMac() { return 0x123456; } };
inline ESPStub ESP;
