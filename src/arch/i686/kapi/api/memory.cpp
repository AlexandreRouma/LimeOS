#include <memory.h>

void* memcpy(void* dstptr, void* srcptr, uint64_t size) {
	uint8_t* dst = (uint8_t*) dstptr;
	uint8_t* src = (uint8_t*) srcptr;
	if (dst < src) {
		for (uint64_t i = 0; i < size; i++)
			dst[i] = src[i];
	} else {
		for (uint64_t i = size; i != 0; i--)
			dst[i-1] = src[i-1];
	}
	return dstptr;
}

bool memcmp(void* aptr, void* bptr, uint64_t size) {
	uint8_t* a = (uint8_t*) aptr;
	uint8_t* b = (uint8_t*) bptr;
	for (uint32_t i = 0; i < size; i++) {
		if (a[i] != b[i]) {
			return false;
		}
	}
	return true;
}

void* memset(void* bufptr, uint8_t value, uint64_t size) {
	uint8_t* buf = (uint8_t*) bufptr;
	for (uint64_t i = 0; i < size; i++) {
		buf[i] = value;
	}	
	return bufptr;
}