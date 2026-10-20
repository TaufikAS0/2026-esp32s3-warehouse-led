#pragma once
#include <cstring>
inline void esp_fill_random(void* data, size_t size) { memset(data, 0x5a, size); }
