#include <stdint.h>
#include <misc/cpuio.h>
#include <misc/memory.h>
#include <context/switch_context.h>
#include <kernio/kernio.h>

#define SLOT_COUNT  256

#define BochsBreak() outw(0x8A00,0x8A00); outw(0x8A00,0x08AE0);

struct TastState_t {
    uint32_t esp;
    bool paused;
    bool used;
    uint32_t priority;
};

TastState_t tasks[SLOT_COUNT];
uint32_t currentId = 0;
uint32_t highId = 0;
bool first = true;

extern "C"
uint32_t _cpp_switch_task(uint32_t esp) {
    // Save current task
    // TODO: Do something cleaner ffs
    if (first) {
        first = false;
        tasks[0].used = true;
    }
    tasks[currentId].esp = esp;
    if (currentId < highId) {
        currentId++;
        while (tasks[currentId].used == false) {
            currentId++;
            if (currentId >= highId) {
                currentId = 0;
            }
        }
    }
    else {
        currentId = 0;
    }
    return tasks[currentId].esp;
}

namespace scheduler {
    int findSlot() {
        for (int i = 0; i < SLOT_COUNT; i++) {
            if (!tasks[i].used) {
                return i;
            }
        }
        return -1;
    }

    Task_t createTask(uint32_t entry, uint32_t stackTop, uint32_t stackSize, uint32_t priority, uint32_t parentPID) {
        TastState_t state;
        state.used = true;
        state.esp = stackTop;
        state.paused = false;
        uint32_t* _stack = (uint32_t*)(stackTop); // EAX -> EDX + ESP -> EDI + EIP
        _stack[3] = stackTop; // esp
        _stack[9] = entry; // eip
        _stack[10] = 8; // CS
        _stack[11] = 1 << 9; // EFLAGS with interrupt bit set
        int id = findSlot();
        tasks[id] = state;
        if (id > highId) {
            highId = id;
        }
        kio::printf("Started task with ID: %u, highId: %u", id, highId);
        Task_t task;
        task.entryPoint = entry;
        task.id = id;
        task.parentProcessId = parentPID;
        task.priority = priority;
        task.stackTop = stackTop;
        task.stackSize = stackSize;
        return task;
    }

    void endTask(Task_t task) {
        tasks[task.id].used = false;
    }
}