#pragma once
#include <stdint.h>

void* memcpy(void* dstptr, void* srcptr, uint64_t size);
bool memcmp(void* aptr, void* bptr, uint64_t size);
void* memset(void* bufptr, uint8_t value, uint64_t size);