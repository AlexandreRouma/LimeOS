#pragma once
#include <stdint.h>

struct Task_t {
    uint32_t id;
    uint32_t parentProcessId;
    uint32_t entryPoint;
    uint32_t priority;
    uint32_t stackTop;
    uint32_t stackBase;
    uint32_t stackSize;
};