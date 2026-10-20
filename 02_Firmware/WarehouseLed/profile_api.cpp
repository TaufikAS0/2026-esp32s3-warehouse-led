#include "web_service.h"
#include <memory>
namespace {
void encodeProfile(const Profiles::Record& record, unsigned id, uint32_t revision, JsonDocument& doc) {
  doc["format"] = 1; doc["id"] = id; doc["name"] = record.name; doc["profile_revision"] = revision;
  Codec::encodeConfig(record.config, doc["config"].to<JsonObject>());
}
bool decodeProfile(JsonVariantConst doc, Profiles::Record& record, String& error) {
  if (!doc["format"].is<unsigned>() || doc["format"].as<unsigned>() != 1 || !doc["name"].is<const char*>()) {
    error = "profile format 1 and name required"; return false;
  }
  const JsonString name = doc["name"].as<JsonString>();
  if (name.size() > 48 || name.size() != strlen(name.c_str()) || !Profiles::validName(name.c_str())) {
    error = "profile name must be 1..48 UTF-8 bytes, trimmed, without control characters"; return false;
  }
  strcpy(record.name, name.c_str());
  const auto config = doc["config"].as<JsonObjectConst>();
  if (config.isNull()) { error = "profile config required"; return false; }
  for (auto field : config) {
    const char* key = field.key().c_str();
    if (strcmp(key, "schema") && strcmp(key, "outputs") && strcmp(key, "brightness_limit") && strcmp(key, "boxes")) {
      error = "profile config accepts mapping only; no Wi-Fi, credentials or runtime state"; return false;
    }
  }
  return Codec::decodeConfig(config, record.config, error);
}
}
bool WebService::checkRevision(JsonVariantConst doc) {
  if (!doc["revision"].is<uint32_t>()) { error(428, "config revision required; GET config first"); return false; }
  if (doc["revision"].as<uint32_t>() != store_.revision()) { error(409, "configuration changed; reload before saving"); return false; }
  return true;
}
void WebService::profileError(Profiles::Result result) {
  using R = Profiles::Result;
  error(result == R::Missing ? 404 : result == R::Corrupt ? 422 : result == R::Invalid ? 400 :
        result == R::Duplicate || result == R::Full || result == R::Conflict ? 409 : 503, Profiles::message(result));
}
void WebService::profileRoute() {
  const String root = "/api/v1/profiles";
  const String path = server_.uri();
  unsigned id = 0; bool apply = false;
  if (path != root) {
    const String suffix = path.substring(root.length() + 1);
    if ((suffix.length() != 1 && suffix.length() != 7) || suffix[0] < '1' || suffix[0] > '5' ||
        (suffix.length() == 7 && suffix.substring(1) != "/apply")) { error(404, "profile endpoint not found"); return; }
    id = suffix[0] - '0'; apply = suffix.length() == 7;
  }
  const auto method = server_.method();
  if ((apply && method != HTTP_POST) ||
      (!apply && !id && method != HTTP_GET && method != HTTP_POST) ||
      (!apply && id && method != HTTP_GET && method != HTTP_PUT && method != HTTP_DELETE)) {
    error(405, "unsupported profile method"); return;
  }
  auto record = std::unique_ptr<Profiles::Record>(new(std::nothrow) Profiles::Record);
  if (!record) { error(503, "profile allocation failed"); return; }
  if (!id && method == HTTP_GET) {
    JsonDocument response; response["capacity"] = Profiles::kSlots;
    auto items = response["profiles"].to<JsonArray>();
    for (unsigned slot = 1; slot <= Profiles::kSlots; ++slot) {
      uint32_t rev; const auto result = store_.profiles.read(slot, *record, rev);
      if (result == Profiles::Result::Missing) continue;
      if (result != Profiles::Result::Ok && result != Profiles::Result::Corrupt) { profileError(result); return; }
      auto item = items.add<JsonObject>(); item["id"] = slot; item["profile_revision"] = rev;
      item["valid"] = result == Profiles::Result::Ok;
      if (result == Profiles::Result::Ok) { item["name"] = record->name; item["box_count"] = record->config.boxCount; }
      else item["error"] = Profiles::message(result);
    }
    send(200, response); return;
  }
  uint32_t revision = 0;
  if (id && method == HTTP_GET) {
    const auto result = store_.profiles.read(id, *record, revision);
    if (result != Profiles::Result::Ok) { profileError(result); return; }
    JsonDocument response; encodeProfile(*record, id, revision, response); send(200, response); return;
  }
  JsonDocument doc; if (!body(doc)) return;
  if (id && !doc["profile_revision"].is<uint32_t>()) { error(428, "profile revision required; reload profiles"); return; }
  if (apply) {
    if (!checkRevision(doc.as<JsonVariantConst>())) return;
    const auto result = store_.profiles.read(id, *record, revision);
    if (result != Profiles::Result::Ok) { profileError(result); return; }
    if (revision != doc["profile_revision"].as<uint32_t>()) { profileError(Profiles::Result::Conflict); return; }
    if (!store_.save(record->config, store_.ssid(), store_.pass())) { error(503, "configuration write failed; previous config active"); return; }
    controller_.configure(record->config); leds_.off(); getConfig(); return;
  }
  if (method == HTTP_DELETE) {
    const auto result = store_.profiles.erase(id, doc["profile_revision"].as<uint32_t>());
    if (result != Profiles::Result::Ok) { profileError(result); return; }
    JsonDocument response; response["ok"] = true; send(200, response); return;
  }
  String e; if (!decodeProfile(doc.as<JsonVariantConst>(), *record, e)) { error(400, e); return; }
  const bool create = id == 0;
  const auto result = store_.profiles.save(id, *record, doc["profile_revision"] | 0u, revision);
  if (result != Profiles::Result::Ok) { profileError(result); return; }
  JsonDocument response; encodeProfile(*record, id, revision, response); send(create ? 201 : 200, response);
}
