#pragma once
#define WAREHOUSE_FIRMWARE_VERSION "v0.7.1"
#include "generated_build.h"
namespace Firmware {
constexpr const char* kVersion = WAREHOUSE_FIRMWARE_VERSION;
constexpr const char* kBuildUtc = WAREHOUSE_BUILD_UTC;
constexpr const char* kBuildIdentity = WAREHOUSE_BUILD_IDENTITY;
}
