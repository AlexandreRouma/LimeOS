#include <stdint.h>
#include <misc/cpuio.h>
#include <misc/memory.h>
#include <context/switch_context.h>
#include <kernio/kernio.h>

#define BochsBreak() outw(0x8A00,0x8A00); outw(0x8A00,0x08AE0);

struct TastState_t {
    uint32_t esp;
};

TastState_t tasks[0x100];
uint32_t currentId = 0;
uint32_t taskCount = 1;

extern "C"
uint32_t _cpp_switch_task(uint32_t esp) {
    tasks[currentId].esp = esp;
    if (currentId < taskCount -1) {
        currentId++;
    }
    else {
        currentId = 0;
    }
    BochsBreak();
    return tasks[currentId].esp;
}

void addTask(uint32_t ptr, uint32_t stack) {
    tasks[taskCount].esp = stack;
    uint32_t* _stack = (uint32_t*)stack; // EAX -> EDX + ESP -> EDI + EIP
    _stack[3] = stack; // esp
    _stack[9] = ptr; // eip
    taskCount++;
}