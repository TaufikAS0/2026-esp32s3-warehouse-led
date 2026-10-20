// Actual SettingsStore and ArduinoJson; in-memory Preferences/serial/RNG boundary only.
#include "settings_store.h"
#include "codec.h"
#include <cassert>
#include <iostream>
int main() {
  Preferences seed;
  JsonDocument auth; auth["token"] = std::string(64, 'a'); auth["ap"] = "old-ap-password"; auth["ota"] = std::string(32, 'b');
  String raw; serializeJson(auth, raw); seed.putString("credentials", raw);
  auto config = Warehouse::defaults(); strcpy(config.boxes[0].id, "legacy-id");
  config.boxes[0].color[0] = 76; config.boxes[0].color[1] = 215; config.boxes[0].color[2] = 246;
  JsonDocument doc; Codec::encodeConfig(config, doc.to<JsonObject>());
  doc["wifi"]["ssid"] = "old-network"; doc["wifi"]["password"] = "old-test-password";
  raw.clear(); serializeJson(doc, raw); seed.putString("config", raw);
  SettingsStore store; Warehouse::Config loaded;
  assert(store.begin(loaded) && store.ready() && store.revision() == 0);
  Preferences::failKey = "config_blob"; const auto oldConfig = Preferences::records["config"];
  assert(!store.migrateLab(loaded));
  assert(Preferences::records["config"] == oldConfig && store.ssid() == "old-network");
  Preferences::failKey.clear();
  SettingsStore retry; assert(retry.begin(loaded) && retry.migrateLab(loaded));
  assert(retry.revision() == 1 && retry.ssid() == "HuaweiJIN");
  assert(retry.apPass() == "12345678" && retry.otaPass() == std::string(32, 'b'));
  assert(!strcmp(loaded.boxes[0].id, "legacy-id") && loaded.boxes[0].color[2] == 246);
  const unsigned writes = Preferences::writes;
  SettingsStore rebooted; assert(rebooted.begin(loaded) && rebooted.migrateLab(loaded));
  assert(Preferences::writes == writes && rebooted.revision() == 1);
  const auto beforeAllocationFailure = Preferences::records["config_blob"];
  const auto secretBeforeAllocationFailure = Preferences::records["credentials"];
  String::maxWrite = 16;
  assert(!rebooted.save(loaded, "another-network", "test-password"));
  assert(!rebooted.setApPassword("new-test-password"));
  assert(Preferences::records["config_blob"] == beforeAllocationFailure && rebooted.revision() == 1);
  assert(Preferences::records["credentials"] == secretBeforeAllocationFailure && rebooted.apPass() == "12345678");
  String::maxWrite = SIZE_MAX;
  Preferences::failKey = "config_blob";
  assert(!rebooted.save(loaded, "another-test-network", "test-password"));
  assert(rebooted.revision() == 1 && rebooted.ssid() == "HuaweiJIN");
  Preferences::failKey.clear();
  // Full 100-box JSON exceeds the real NVS string limit; blob save/re-save/reboot must work.
  auto full = std::unique_ptr<Warehouse::Config>(new Warehouse::Config);
  *full = loaded; full->boxCount = 100;
  for (auto& count : full->ledCounts) count = 75;
  for (unsigned i = 0; i < 100; ++i) {
    auto& b = full->boxes[i];
    snprintf(b.id, sizeof(b.id), "%c-%02u", 'A' + i / 25, 1 + i % 25);
    strcpy(b.name, "-"); b.output = i / 25; b.start = (i % 25) * 3; b.count = 3;
    b.color[0] = 76; b.color[1] = 215; b.color[2] = 246;
  }
  JsonDocument large; Codec::encodeConfig(*full, large.to<JsonObject>());
  String largeRaw; serializeJson(large, largeRaw);
  assert(largeRaw.length() > 4000 && !seed.putString("too_large", largeRaw));
  assert(rebooted.save(*full, rebooted.ssid(), rebooted.pass()));
  for (unsigned i = 0; i < 100; ++i) {
    snprintf(full->boxes[i].id, sizeof(full->boxes[i].id), "ID-%021u", i);
    memset(full->boxes[i].name, 'N', 48); full->boxes[i].name[48] = 0;
  }
  assert(rebooted.save(*full, rebooted.ssid(), rebooted.pass()));
  assert(Preferences::records["config"] == oldConfig); // Legacy record never destroyed.
  SettingsStore largeReboot; assert(largeReboot.begin(loaded));
  assert(loaded.boxCount == 100 && loaded.boxes[99].start == 72 && largeReboot.revision() == 3);
  const auto savedBlob = Preferences::records["config_blob"];
  Preferences::failKey = "config_blob";
  assert(!largeReboot.save(*full, "different-network", "test-password"));
  assert(Preferences::records["config_blob"] == savedBlob && largeReboot.revision() == 3);
  Preferences::failKey.clear();
  auto record = std::unique_ptr<Profiles::Record>(new Profiles::Record);
  record->config = loaded; strcpy(record->name, "Saved");
  unsigned id = 0; uint32_t revision;
  assert(rebooted.profiles.save(id, *record, 0, revision) == Profiles::Result::Ok);
  assert(Preferences::records["config"] != std::vector<uint8_t>{});
  const auto credentials = Preferences::records["credentials"];
  seed.putBytes("config_blob", "{broken", 7);
  SettingsStore corrupt; assert(!corrupt.begin(loaded) && !corrupt.migrateLab(loaded));
  assert(Preferences::records["config"] == oldConfig && Preferences::records["credentials"] == credentials);
  assert(loaded.boxCount == 1 && !strcmp(loaded.boxes[0].id, "R01-B01") && loaded.boxes[0].color[1] == 255);
  assert(corrupt.profiles.read(id, *record, revision) == Profiles::Result::Ok);
  std::cout << "SettingsStore tests passed: legacy NVS, revision, targeted lab migration, failures, corrupt config isolation\n";
}
