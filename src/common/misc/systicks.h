#pragma once
#include <stdint.h>

namespace systicks {
    uint64_t getTicks();
    void setTicks(uint64_t ticks);
    void increment();
}
