#include <misc/random.h>

namespace rand {
    uint32_t get() {
        seed = seed * 1103515245 +12345;
        return seed;
    }
    uint64_t seed;
}