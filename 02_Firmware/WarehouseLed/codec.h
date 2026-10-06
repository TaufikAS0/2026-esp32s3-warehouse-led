#pragma once
#include <ArduinoJson.h>
#include "domain.h"
namespace Codec {
bool decodeConfig(JsonVariantConst root, Warehouse::Config& out, String& error);
void encodeConfig(const Warehouse::Config& config, JsonObject out);
bool decodeCommand(JsonVariantConst root, Warehouse::Command& out, String& error);
bool authenticated(const String& header, const String& token);
}
