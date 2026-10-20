#pragma once
#include <string>
#include <stdint.h>
class String : public std::string {
 public:
  using std::string::string;
  using std::string::operator=;
  String(const std::string& s) : std::string(s) {}
  bool isEmpty() const { return empty(); }
  size_t length() const { return size(); }
  inline static size_t maxWrite = SIZE_MAX;
  size_t write(uint8_t byte) { if (size() >= maxWrite) return 0; push_back(char(byte)); return 1; }
  size_t write(const uint8_t* bytes, size_t n) {
    const size_t available = size() < maxWrite ? maxWrite - size() : 0;
    if (n > available) n = available;
    append(reinterpret_cast<const char*>(bytes), n); return n;
  }
  bool startsWith(const String& prefix) const { return rfind(prefix, 0) == 0; }
  bool endsWith(const String& suffix) const { return size() >= suffix.size() && compare(size() - suffix.size(), suffix.size(), suffix) == 0; }
  String substring(size_t start, size_t end) const { return String(substr(start, end - start)); }
  String substring(size_t start) const { return String(substr(start)); }
};
struct SerialStub {
  void println(const char*) {}
  void println(const String&) {}
  template<class... Args> void printf(const char*, Args...) {}
};
inline SerialStub Serial;
#include <ArduinoJson.h>
namespace ArduinoJson {
template<> struct Converter<String> {
  static String fromJson(JsonVariantConst src) { return String(src.as<std::string>()); }
  static void toJson(const String& src, JsonVariant dest) { dest.set(static_cast<const std::string&>(src)); }
  static bool checkJson(JsonVariantConst src) { return src.is<std::string>(); }
};
}
