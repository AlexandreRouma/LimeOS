#include <misc/systicks.h>

namespace systicks {
    uint64_t _ticks;

    uint64_t getTicks() {
        return _ticks;
    }

    void setTicks(uint64_t ticks) {
        _ticks = ticks;
    }

    void increment() {
        _ticks++;
    }
}