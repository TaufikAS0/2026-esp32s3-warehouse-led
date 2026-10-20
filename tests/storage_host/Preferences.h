#pragma once
#include "Arduino.h"
#include <map>
#include <vector>
#include <cstring>
class Preferences {
 public:
  inline static std::map<std::string, std::vector<uint8_t>> records;
  inline static unsigned writes = 0;
  inline static std::string failKey;
  inline static std::map<std::string, bool> stringTypes;
  bool begin(const char*, bool) { return true; }
  bool isKey(const char* key) { return records.count(key); }
  String getString(const char* key) {
    if (!records.count(key) || !stringTypes[key]) return "";
    auto& raw = records[key]; return String(std::string(raw.begin(), raw.end()));
  }
  size_t putString(const char* key, const String& s) {
    // Real nvs_set_str is limited to 4000 bytes including the terminating NUL.
    if (s.size() + 1 > 4000 || (records.count(key) && !stringTypes[key])) return 0;
    if (failKey == key) return 0;
    const auto raw = reinterpret_cast<const uint8_t*>(s.data());
    records[key] = std::vector<uint8_t>(raw, raw + s.size());
    stringTypes[key] = true; ++writes; return s.size();
  }
  size_t putBytes(const char* key, const void* bytes, size_t size) {
    if (failKey == key || (records.count(key) && stringTypes[key])) return 0;
    const auto raw = static_cast<const uint8_t*>(bytes);
    records[key] = std::vector<uint8_t>(raw, raw + size); stringTypes[key] = false; ++writes; return size;
  }
  size_t getBytesLength(const char* key) { return records.count(key) && !stringTypes[key] ? records[key].size() : 0; }
  size_t getBytes(const char* key, void* out, size_t size) {
    if (getBytesLength(key) != size) return 0;
    memcpy(out, records[key].data(), size); return size;
  }
  bool remove(const char* key) { return failKey != key && records.erase(key); }
};
