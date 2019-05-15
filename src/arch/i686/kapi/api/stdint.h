#pragma once
#define INT8_MIN        (-128)
#define INT16_MIN       (-32767-1)
#define INT32_MIN       (-2147483647-1)
#define INT64_MIN       (-__INT64_C(9223372036854775807)-1)

#define INT8_MAX        (127)
#define INT16_MAX       (32767)
#define INT32_MAX       (2147483647)
#define INT64_MAX       (__INT64_C(9223372036854775807))

#define UINT8_MAX       (255)
#define UINT16_MAX      (65535)
#define UINT32_MAX      (4294967295U)
#define UINT64_MAX      (__UINT64_C(18446744073709551615))

#define NULL            0

typedef signed char             int8_t;
typedef short int               int16_t;
typedef int                     int32_t;
typedef long int                int64_t;
//typedef long long int           int64_t;
typedef unsigned char           uint8_t;
typedef unsigned short int      uint16_t;
typedef unsigned int            uint32_t;
typedef unsigned long long int  uint64_t;

// TODO: Add GCC specific byteflip functions to increase speed

inline uint16_t le_uint16(uint8_t* ptr) {
    return ((uint16_t)ptr[0] << 8) | ptr[1];
}

inline uint32_t le_uint32(uint8_t* ptr) {
    return ((uint32_t)ptr[0] << 24) | ((uint32_t)ptr[1] << 16) | ((uint32_t)ptr[2] << 8) | ptr[3];
}

inline uint64_t le_uint64(uint8_t* ptr) {
    return ((uint64_t)ptr[0] << 56) | ((uint64_t)ptr[1] << 48) | ((uint64_t)ptr[2] << 40) | ((uint64_t)ptr[3] << 32) | ((uint64_t)ptr[4] << 24) | ((uint64_t)ptr[5] << 16) | ((uint64_t)ptr[6] << 8) | ptr[7];
}

inline uint16_t be_uint16(uint8_t* ptr) {
    return ((uint16_t)ptr[1] << 8) | ptr[0];
}

inline uint32_t be_uint32(uint8_t* ptr) {
    return ((uint32_t)ptr[3] << 24) | ((uint32_t)ptr[2] << 16) | ((uint32_t)ptr[1] << 8) | ptr[0];
}

inline uint64_t be_uint64(uint8_t* ptr) {
    return ((uint64_t)ptr[7] << 56) | ((uint64_t)ptr[6] << 48) | ((uint64_t)ptr[5] << 40) | ((uint64_t)ptr[4] << 32) | ((uint64_t)ptr[3] << 24) | ((uint64_t)ptr[2] << 16) | ((uint64_t)ptr[1] << 8) | ptr[0];
}