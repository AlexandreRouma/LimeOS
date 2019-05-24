#pragma once

extern "C" {
    extern void _ASM_SWITCH_TASK_ISR(void);
    extern void _ASM_YIELD(void);
}

void addTask(uint32_t ptr, uint32_t stack);

struct Task_t {
    uint32_t id;
    uint32_t parentProcessId;
    uint32_t entryPoint;
    uint32_t priority;
    uint32_t stackTop;
    uint32_t stackSize;
};

namespace scheduler {
    void init();
    Task_t createTask(uint32_t entry, uint32_t stackTop, uint32_t stackSize, uint32_t priority, uint32_t parentPID);
    void endTask(Task_t task);
    void yield();
    void sleep(uint32_t ticks);
}