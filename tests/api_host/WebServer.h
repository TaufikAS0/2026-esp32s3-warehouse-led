#pragma once
#include "Arduino.h"
#include <functional>
#include <map>
enum HTTPMethod { HTTP_GET, HTTP_PUT, HTTP_POST, HTTP_DELETE };
// Calls real handlers with in-memory request/response; no socket/timing emulation.
class WebServer {
 public:
  inline static WebServer* instance = nullptr;
  int code=0;
  String response;
  explicit WebServer(int) { instance = this; }
  void on(const char* path, HTTPMethod method, std::function<void()> action) { routes_[{path,method}] = action; }
  void onNotFound(std::function<void()> action) { unknown_ = action; }
  void collectHeaders(const char**, size_t) {}
  void sendHeader(const char*, const char*) {}
  void send(int codeValue, const char*, const String& text) { code=codeValue; response=text; }
  void send_P(int codeValue, const char*, const char* text) { code=codeValue; response=text; }
  void begin() {}
  void close() {}
  void handleClient() {}
  HTTPMethod method() const { return method_; }
  String uri() const { return uri_; }
  String header(const char*) const { return contentType_; }
  const String& arg(const char*) const { return body_; }
  void request(HTTPMethod methodValue, const String& path, const String& body="", const String& contentType="application/json") {
    method_=methodValue;uri_=path;body_=body;contentType_=contentType;code=0;response="";
    auto action=routes_.find({std::string(path),methodValue});
    if(action!=routes_.end()) action->second(); else unknown_();
  }
 private:
  HTTPMethod method_=HTTP_GET;
  String uri_,body_,contentType_;
  std::map<std::pair<std::string,HTTPMethod>,std::function<void()>> routes_;
  std::function<void()> unknown_;
};
