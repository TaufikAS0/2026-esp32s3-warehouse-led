#include "profile_store.h"
#include <cassert>
#include <iostream>
#include <map>
#include <string>
#include <vector>
struct Nvs {
  std::map<std::string, std::vector<uint8_t>> records;
  bool fail = false;
  bool failWrite = false;
  size_t writes = 0;
  bool isKey(const char* key) { return records.count(key); }
  size_t getBytesLength(const char* key) { return records[key].size(); }
  size_t getBytes(const char* key, void* out, size_t n) {
    if (fail) return 0;
    memcpy(out, records[key].data(), n); return n;
  }
  size_t putBytes(const char* key, const void* raw, size_t n) {
    size_t occupied = 0;
    for (const auto& item : records) occupied += (item.second.size() + 31) / 32 * 32 + 64;
    // Copy-on-write reserves room for old + new record before reclaiming old.
    if (fail || failWrite || occupied + n + 64 > 120 * 1024) return 0;
    records[key] = std::vector<uint8_t>((const uint8_t*)raw, (const uint8_t*)raw + n);
    ++writes; return n;
  }
  bool remove(const char* key) { return !fail && records.erase(key); }
};
int main() {
  using namespace Profiles;
  Nvs nvs; Repository<Nvs> repo(nvs);
  auto record = std::unique_ptr<Record>(new Record);
  record->config.boxCount = 100;
  for (unsigned o = 0; o < 4; ++o) {
    record->config.ledCounts[o] = 75;
    for (unsigned b = 0; b < 25; ++b) {
      auto& box = record->config.boxes[o * 25 + b];
      snprintf(box.id, sizeof(box.id), "R%02u-B%02u-abcdefghijklmnop", o + 1, b + 1);
      memset(box.name, 'x', 48); box.name[48] = 0;
      box.output = o; box.start = b * 3; box.count = 3;
      box.color[0] = 76; box.color[1] = 215; box.color[2] = 246;
    }
  }
  std::vector<uint8_t> raw(kMaxBytes); memset(record->name, 'p', 48); record->name[48] = 0;
  size_t size = encode(*record, raw.data()); assert(size == 8065 && size <= 8192);
  Record decoded;
  assert(decode(raw.data(), size, decoded));
  assert(decoded.config.boxCount == 100 && decoded.config.boxes[99].start == 72);
  raw[size - 1] ^= 1; assert(!decode(raw.data(), size, decoded)); raw[size - 1] ^= 1;
  assert(!decode(raw.data(), size - 1, decoded));
  raw[0] ^= 1; assert(!decode(raw.data(), size, decoded)); raw[0] ^= 1;
  assert(!validName("") && !validName(" untrimmed") && !validName("line\nbreak"));
  assert(!validName("\xc0\xaf") && validName("Gudang utama"));
  uint32_t revisions[5]{};
  // Worst config allowance plus five maximum-size profiles and replacement room.
  nvs.records["config"] = std::vector<uint8_t>(32768, 0);
  nvs.records["credentials"] = std::vector<uint8_t>(256, 0);
  for (unsigned i = 1; i <= 5; ++i) {
    memset(record->name, 'p', 48); record->name[47] = char('0' + i); record->name[48] = 0;
    unsigned id = 0;
    assert(repo.save(id, *record, 0, revisions[i - 1]) == Result::Ok && id == i);
  }
  Repository<Nvs> rebooted(nvs);
  for (unsigned i = 1; i <= 5; ++i) {
    uint32_t revision;
    assert(rebooted.read(i, decoded, revision) == Result::Ok && revision == revisions[i - 1]);
    assert(decoded.config.boxCount == 100 && decoded.config.boxes[99].color[2] == 246);
  }
  strcpy(record->name, "Sixth"); unsigned id = 0; uint32_t revision;
  assert(repo.save(id, *record, 0, revision) == Result::Full);
  id = 1; memset(record->name, 'p', 48); record->name[47] = '2'; record->name[48] = 0;
  assert(repo.save(id, *record, revisions[0], revision) == Result::Duplicate);
  strcpy(record->name, "Updated");
  assert(repo.save(id, *record, revisions[0] ^ 1, revision) == Result::Conflict);
  const auto previous = nvs.records["profile1"];
  nvs.failWrite = true;
  assert(repo.save(id, *record, revisions[0], revision) == Result::Storage);
  assert(nvs.records["profile1"] == previous);
  nvs.failWrite = false;
  assert(repo.save(id, *record, revisions[0], revision) == Result::Ok && revision != revisions[0]);
  assert(repo.erase(1, revisions[0]) == Result::Conflict);
  assert(repo.erase(1, revision) == Result::Ok);
  nvs.records["profile2"].back() ^= 1;
  assert(repo.read(2, decoded, revision) == Result::Corrupt);
  assert(repo.erase(2, revision) == Result::Ok);
  assert(repo.read(3, decoded, revision) == Result::Ok); // corruption isolated
  id = 0; assert(repo.save(id, *record, 0, revision) == Result::Ok && id == 1);
  std::cout << "Profile tests passed: 5x100 boxes, compact codec, CRC, COW budget, failures, conflicts, reboot\n";
}
