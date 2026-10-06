#pragma once
#include <string>
// Minimal host adapter for the Arduino String operations used by codec.cpp.
// JSON implementation itself is the pinned real ArduinoJson library, not a mock.
class String : public std::string {
 public:
  using std::string::string;
  using std::string::operator=;
  String(const std::string& value) : std::string(value) {}
  size_t length() const { return size(); }
};
