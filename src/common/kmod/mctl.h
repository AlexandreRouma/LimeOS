#pragma once
#include <stdint.h>

struct MCTLHandler_t {
    int (*_handler)(void* tag, uint32_t id, void* in,void* out);
    void* tag;
};

namespace mctl {
    void init();
    void registerHndlr(char* dev, MCTLHandler_t handler);
    int call(char* dev, uint32_t id, void* in, void* out);
}