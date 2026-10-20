#include "web_service.h"
#include <ArduinoOTA.h>
#include <cassert>
#include <iostream>
int main() {
  Warehouse::Controller controller; SettingsStore store; LedDriver leds; WifiService wifi; OtaService ota;
  WebService web(controller,store,leds,wifi,ota);
  auto config = Warehouse::defaults(); assert(store.begin(config)); assert(store.provision());
  controller.configure(config); assert(leds.begin()); wifi.begin(store); wifi.update(0); ota.begin(store,wifi); web.begin();
  auto& server=*WebServer::instance;
  JsonDocument response;
  auto request = [&](HTTPMethod method,const String& path,const String& body,int code) {
    server.request(method,path,body);if(server.code!=code)std::cerr<<"HTTP "<<server.code<<" expected "<<code<<" at "<<path<<"\n";
    assert(server.code==code);response.clear();assert(!deserializeJson(response,server.response));
  };
  request(HTTP_GET,"/api/v1/config","",200);assert(response["revision"].as<unsigned>()==0);
  request(HTTP_PUT,"/api/v1/config","{}",428);
  request(HTTP_PUT,"/api/v1/config","{\"revision\":999}",409);
  config.boxCount=2;config.ledCounts[0]=75;config.ledCounts[3]=75;
  config.boxes[0].count=75;config.boxes[1]=config.boxes[0];config.boxes[1].output=3;strcpy(config.boxes[1].id,"Z");
  JsonDocument candidate;Codec::encodeConfig(config,candidate.to<JsonObject>());candidate["revision"]=0;
  candidate["wifi"]["ssid"]="fixture-network";candidate["wifi"]["password"]="fixture-password";
  auto json = [](const JsonDocument& doc){String text;serializeJson(doc,text);return text;};
  request(HTTP_PUT,"/api/v1/config",json(candidate),200);assert(store.revision()==1);
  request(HTTP_PUT,"/api/v1/boxes/R01-B01/state","{\"on\":true}",200);
  request(HTTP_PUT,"/api/v1/program","{\"on\":true,\"pattern\":\"rainbow\",\"idle\":true,\"step_ms\":100,\"brightness\":255}",200);
  assert(!controller.state(0).on&&response["program"]["output"].as<unsigned>()==1);
  assert(response["program"]["brightness"].as<unsigned>()==128);
  assert(response["boxes"][0]["program_colors"].size()==75);
  assert(response["boxes"][0]["program_colors"][0][0].as<unsigned>()==128);
  assert(response["boxes"][0]["program_colors"][25][1].as<unsigned>()==128);
  assert(response["boxes"][0]["program_colors"][50][2].as<unsigned>()==128);
  Adafruit_NeoPixel::enforceSingleRack=true;
  testNow=600;leds.update(controller,testNow);testNow=1200;leds.update(controller,testNow);
  request(HTTP_PUT,"/api/v1/outputs/4/test","{\"on\":true}",200);assert(!leds.program().active(testNow));
  request(HTTP_PUT,"/api/v1/outputs/1/test","{\"on\":true}",200);assert(!leds.hardwareTests().remaining(3,testNow));
  request(HTTP_PUT,"/api/v1/boxes/Z/state","{\"on\":true}",200);
  assert(!leds.program().active(testNow)&&!leds.hardwareTests().active(testNow)&&controller.state(1).on);
  // Rejected config/apply must leave the manual command running.
  candidate["revision"]=1;Preferences::failKey="config_blob";
  request(HTTP_PUT,"/api/v1/config",json(candidate),503);assert(store.revision()==1&&controller.state(1).on);
  Preferences::failKey.clear();
  JsonDocument envelope;envelope["format"]=1;envelope["name"]="Fixture";Codec::encodeConfig(config,envelope["config"].to<JsonObject>());
  request(HTTP_POST,"/api/v1/profiles",json(envelope),201);assert(controller.state(1).on);
  const uint32_t profileRevision=response["profile_revision"];
  request(HTTP_GET,"/api/v1/profiles/1","",200);assert(response["config"]["wifi"].isUnbound());
  request(HTTP_POST,"/api/v1/profiles",json(envelope),409);
  request(HTTP_POST,"/api/v1/profiles/1/apply","{\"revision\":1}",428);
  JsonDocument apply;apply["revision"]=0;apply["profile_revision"]=profileRevision;
  request(HTTP_POST,"/api/v1/profiles/1/apply",json(apply),409);
  apply["revision"]=1;Preferences::failKey="config_blob";
  request(HTTP_POST,"/api/v1/profiles/1/apply",json(apply),503);assert(controller.state(1).on);
  Preferences::failKey.clear();const String ssid=store.ssid(),pass=store.pass(),otaPass=store.otaPass();
  request(HTTP_POST,"/api/v1/profiles/1/apply",json(apply),200);
  assert(store.revision()==2&&!controller.state(1).on&&store.ssid()==ssid&&store.pass()==pass&&store.otaPass()==otaPass);
  assert(store.ssid()=="fixture-network"&&store.pass()=="fixture-password");
  for(unsigned i=2;i<=5;++i){envelope["name"]=std::string("Fixture ")+char('0'+i);request(HTTP_POST,"/api/v1/profiles",json(envelope),201);}
  envelope["name"]="Sixth";request(HTTP_POST,"/api/v1/profiles",json(envelope),409);
  envelope["config"]["wifi"]["ssid"]="must reject";request(HTTP_PUT,"/api/v1/profiles/1",json(envelope),428);
  envelope["profile_revision"]=profileRevision;request(HTTP_PUT,"/api/v1/profiles/1",json(envelope),400);
  envelope["config"].remove("wifi");Preferences::failKey="profile1";
  request(HTTP_PUT,"/api/v1/profiles/1",json(envelope),503);Preferences::failKey.clear();
  request(HTTP_GET,"/api/v1/profiles/1","",200);assert(response["name"]=="Fixture");
  Preferences::records["profile1"].back()^=1;
  request(HTTP_GET,"/api/v1/profiles/1","",422);
  request(HTTP_GET,"/api/v1/profiles","",200);assert(!response["profiles"][0]["valid"].as<bool>());
  JsonDocument erase;erase["profile_revision"]=response["profiles"][0]["profile_revision"].as<uint32_t>();
  request(HTTP_DELETE,"/api/v1/profiles/1",json(erase),200);
  request(HTTP_GET,"/api/v1/profiles/1","",404);
  request(HTTP_GET,"/api/v1/profiles/6","",404);
  request(HTTP_GET,"/api/v1/profiles/2/apply","",405);
  request(HTTP_POST,"/api/v1/ota","{\"enabled\":true}",200);assert(ArduinoOTA.enabled);
  WiFi.connected=false;ota.update(testNow);assert(!ArduinoOTA.enabled);
  std::cout<<"Real handler API tests passed: config/profile revisions, CRUD/apply failures, exclusive tests/manual, OTA guard\n";
}
