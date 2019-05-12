#pragma once
#include <stdint.h>

namespace rand {
    uint32_t get();
    extern uint64_t seed;
};