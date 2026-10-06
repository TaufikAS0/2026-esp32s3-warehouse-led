#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define PROGMEM
#define INPUT 0
#define OUTPUT 1
#define LOW 0
#define ESP_IDF_VERSION_VAL(major, minor, patch) ((major)*10000+(minor)*100+(patch))
#define ESP_IDF_VERSION ESP_IDF_VERSION_VAL(5, 5, 0)
inline uint8_t pgm_read_byte(const uint8_t* p) { return *p; }
inline void pinMode(int, int) {}
inline void digitalWrite(int, int) {}
inline uint32_t micros() { static uint32_t now = 0; return now += 1000; }
inline void interrupts() {}
inline void noInterrupts() {}
