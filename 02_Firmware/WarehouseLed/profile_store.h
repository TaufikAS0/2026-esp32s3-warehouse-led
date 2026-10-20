#pragma once
#include "domain.h"
#include <memory>
namespace Profiles {
constexpr size_t kSlots = 5, kMaxBytes = 8192;
struct Record { char name[49]{}; Warehouse::Config config; };
inline bool utf8(const char* s, size_t n) {
  for (size_t i = 0; i < n;) {
    const uint8_t a = s[i++];
    if (!a) return false;
    if (a < 128) continue;
    unsigned extra; uint32_t value, minimum;
    if (a >= 0xc2 && a <= 0xdf) { extra = 1; value = a & 31; minimum = 0x80; }
    else if (a >= 0xe0 && a <= 0xef) { extra = 2; value = a & 15; minimum = 0x800; }
    else if (a >= 0xf0 && a <= 0xf4) { extra = 3; value = a & 7; minimum = 0x10000; }
    else return false;
    if (i + extra > n) return false;
    while (extra--) { uint8_t b = s[i++]; if ((b & 0xc0) != 0x80) return false; value = (value << 6) | (b & 63); }
    if (value < minimum || value > 0x10ffff || (value >= 0xd800 && value <= 0xdfff)) return false;
  }
  return true;
}
inline bool validName(const char* s) {
  const size_t n = strlen(s);
  if (!n || n > 48 || !utf8(s, n) || s[0] == ' ' || s[n - 1] == ' ') return false;
  for (size_t i = 0; i < n; ++i) if (uint8_t(s[i]) < 32 || s[i] == 127) return false;
  return true;
}
inline uint32_t crc32(const uint8_t* bytes, size_t n) {
  uint32_t crc = 0xffffffffu;
  for (size_t i = 0; i < n; ++i) {
    crc ^= bytes[i];
    for (unsigned b = 0; b < 8; ++b) crc = (crc >> 1) ^ (0xedb88320u & (0u - (crc & 1u)));
  }
  return ~crc;
}
inline void put32(uint8_t* p, uint32_t x) { for (unsigned i = 0; i < 4; ++i) p[i] = x >> (i * 8); }
inline uint32_t get32(const uint8_t* p) {
  return uint32_t(p[0]) | uint32_t(p[1]) << 8 | uint32_t(p[2]) << 16 | uint32_t(p[3]) << 24;
}
inline size_t encode(const Record& record, uint8_t* out) {
  if (!validName(record.name) || Warehouse::validate(record.config)) return 0;
  size_t at = 10;
  const auto text = [&](const char* s) {
    size_t n = strlen(s); out[at++] = n; memcpy(out + at, s, n); at += n;
  };
  text(record.name);
  out[at++] = record.config.brightnessLimit;
  for (auto n : record.config.ledCounts) out[at++] = n;
  out[at++] = record.config.boxCount;
  for (size_t i = 0; i < record.config.boxCount; ++i) {
    const auto& b = record.config.boxes[i];
    if (!utf8(b.name, strlen(b.name))) return 0;
    text(b.id); text(b.name); out[at++] = b.output; out[at++] = b.start; out[at++] = b.count;
    for (auto channel : b.color) out[at++] = channel;
  }
  // Maximum: header10 + name49 + limits6 + 100*(ID25+label49+mapping6) = 8065.
  memcpy(out, "WLP1", 4); out[4] = at; out[5] = at >> 8;
  put32(out + 6, crc32(out + 10, at - 10));
  return at;
}
inline bool decode(const uint8_t* raw, size_t n, Record& out) {
  if (n < 17 || n > kMaxBytes || memcmp(raw, "WLP1", 4) ||
      (size_t(raw[4]) | size_t(raw[5]) << 8) != n || get32(raw + 6) != crc32(raw + 10, n - 10)) return false;
  // WLP1 explicitly identifies binary format version 1.
  size_t at = 10;
  const auto text = [&](char* dest, size_t max) {
    if (at >= n) return false;
    size_t len = raw[at++];
    if (len > max || at + len > n || !utf8(reinterpret_cast<const char*>(raw + at), len)) return false;
    memcpy(dest, raw + at, len); dest[len] = 0; at += len; return true;
  };
  if (!text(out.name, 48) || !validName(out.name) || at + 6 > n) return false;
  out.config.brightnessLimit = raw[at++];
  for (auto& count : out.config.ledCounts) count = raw[at++];
  out.config.boxCount = raw[at++];
  if (!out.config.boxCount || out.config.boxCount > Warehouse::kMaxBoxes) return false;
  for (size_t i = 0; i < out.config.boxCount; ++i) {
    auto& b = out.config.boxes[i];
    if (!text(b.id, 24) || !text(b.name, 48) || at + 6 > n) return false;
    b.output = raw[at++]; b.start = raw[at++]; b.count = raw[at++];
    for (auto& c : b.color) c = raw[at++];
  }
  return at == n && !Warehouse::validate(out.config);
}
enum class Result { Ok, Missing, Corrupt, Invalid, Duplicate, Full, Conflict, Storage, Allocation };
inline const char* message(Result r) {
  switch (r) {
    case Result::Missing: return "profile not found";
    case Result::Corrupt: return "profile corrupt; delete or import a backup";
    case Result::Invalid: return "invalid profile name or mapping";
    case Result::Duplicate: return "profile name already exists";
    case Result::Full: return "all five profile slots are occupied";
    case Result::Conflict: return "profile changed; reload before writing";
    case Result::Storage: return "profile storage operation failed; previous record retained";
    case Result::Allocation: return "profile allocation failed";
    default: return "ok";
  }
}
// Backend matches Preferences. Each update is one atomic NVS record, no separate index.
template<class Backend> class Repository {
 public:
  explicit Repository(Backend& backend) : backend_(backend) {}
  bool exists(unsigned id) { char key[12]; return valid(id) && backend_.isKey(keyFor(id, key)); }
  Result read(unsigned id, Record& out, uint32_t& revision) {
    revision = 0;
    if (!exists(id)) return Result::Missing;
    char key[12]; const size_t size = backend_.getBytesLength(keyFor(id, key));
    if (!size || size > kMaxBytes) return Result::Corrupt;
    auto raw = std::unique_ptr<uint8_t[]>(new(std::nothrow) uint8_t[size]);
    if (!raw) return Result::Allocation;
    if (backend_.getBytes(key, raw.get(), size) != size) return Result::Storage;
    revision = crc32(raw.get(), size);
    return decode(raw.get(), size, out) ? Result::Ok : Result::Corrupt;
  }
  Result save(unsigned& id, const Record& record, uint32_t expected, uint32_t& revision) {
    if (!validName(record.name) || Warehouse::validate(record.config)) return Result::Invalid;
    auto other = std::unique_ptr<Record>(new(std::nothrow) Record);
    if (!other) return Result::Allocation;
    unsigned free = 0;
    for (unsigned slot = 1; slot <= kSlots; ++slot) {
      uint32_t rev; Result r = read(slot, *other, rev);
      if (r == Result::Missing) { if (!free) free = slot; continue; }
      if (r == Result::Allocation || r == Result::Storage) return r;
      if (r == Result::Ok && slot != id && !strcmp(other->name, record.name)) return Result::Duplicate;
    }
    if (id) {
      uint32_t current; Result r = read(id, *other, current);
      if (r != Result::Ok && r != Result::Corrupt) return r;
      if (current != expected) return Result::Conflict;
    } else {
      if (!free) return Result::Full;
      id = free;
    }
    auto raw = std::unique_ptr<uint8_t[]>(new(std::nothrow) uint8_t[kMaxBytes]);
    if (!raw) return Result::Allocation;
    size_t size = encode(record, raw.get());
    if (!size) return Result::Invalid;
    char key[12];
    if (backend_.putBytes(keyFor(id, key), raw.get(), size) != size) return Result::Storage;
    revision = crc32(raw.get(), size); return Result::Ok;
  }
  Result erase(unsigned id, uint32_t expected) {
    auto record = std::unique_ptr<Record>(new(std::nothrow) Record);
    if (!record) return Result::Allocation;
    uint32_t revision; Result r = read(id, *record, revision);
    if (r != Result::Ok && r != Result::Corrupt) return r;
    if (revision != expected) return Result::Conflict;
    char key[12]; return backend_.remove(keyFor(id, key)) ? Result::Ok : Result::Storage;
  }
 private:
  static bool valid(unsigned id) { return id >= 1 && id <= kSlots; }
  static const char* keyFor(unsigned id, char* key) { snprintf(key, 12, "profile%u", id); return key; }
  Backend& backend_;
};
}
