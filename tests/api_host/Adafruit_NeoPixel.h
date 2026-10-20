#pragma once
#include "Arduino.h"
#include <cstring>
#define NEO_GRB 0
#define NEO_KHZ800 0
class Adafruit_NeoPixel {
 public:
  inline static uint8_t shown[8][225]{};
  inline static bool enforceSingleRack = false;
  Adafruit_NeoPixel(unsigned n, unsigned pin, unsigned) : n_(n), pin_(pin) {}
  unsigned numPixels() const { return n_; }
  bool begin() { return true; }
  void clear() { memset(pixels_, 0, sizeof(pixels_)); }
  void setPixelColor(unsigned i, uint8_t r, uint8_t g, uint8_t b) { pixels_[i*3]=r; pixels_[i*3+1]=g; pixels_[i*3+2]=b; }
  void show() {
    memcpy(shown[pin_], pixels_, sizeof(pixels_));
    if (enforceSingleRack) {
      unsigned racks=0,lit=0;
      for (unsigned p=4;p<8;++p) { bool on=false;for(unsigned i=0;i<75;++i)if(shown[p][i*3]||shown[p][i*3+1]||shown[p][i*3+2]){on=true;++lit;}racks+=on; }
      if (racks > 1 || lit > 75) abort();
    }
  }
 private:
  unsigned n_, pin_;
  uint8_t pixels_[225]{};
};
