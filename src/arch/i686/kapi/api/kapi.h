#pragma once
#include <stdint.h>
#include <stdarg.h>
#include <stream.h>

#ifdef KERNEL_CODE
#include <multiboot/multiboot.h>
#include <vfs/vfs.h>
#include <kmod/mctl.h>
#include <scheduler/scheduler.h>
#else
#include <multiboot.h>
#include <vfs.h>
#include <mctl.h>
#include <scheduler.h>
#endif

struct FIOAPI_t {
    bool (*createDir)(char*);
    bool (*mountStreamProvider)(char*, uint16_t, stream_t (*)());
};

struct KIOAPI_t {
    void (*print)(char*);
    void (*println)(char*);
};

struct MCTLAPI_t {
    void (*registerHndlr)(char* dev, MCTLHandler_t handler);
    int (*call)(char* dev, uint32_t id, void* in, void* out);
};

struct MMAPI_t {
    void* (*malloc)(uint32_t size);
    void (*free)(void*);
    void* (*realloc)(void*, uint32_t);
    uint32_t (*allocPages)(uint32_t count);
    void (*setPresent)(uint32_t virt, uint32_t count);
};

struct VFSAPI_t {
    vector<FSNode_t> (*listNodes)(char* dir);
    int (*createNode)(char* dir, uint32_t flags);
    int (*deleteNode)(char* dir);
    bool (*nodeExists)(char* dir);
    stream_t (*getStream)(char* dir);
    bool (*registerFSHndlr)(FSHandler_t handler);
};

struct IDTAPI_t {
    void (*encodeIdtEntry)(uint8_t index, uint32_t offset, uint16_t selector, uint8_t type);
    void (*load)();
};

struct SchedulerAPI_t {
    Task_t (*createTask)(uint32_t entry, uint32_t stackSize, uint32_t priority, uint32_t parentPID);
    void (*endTask)(Task_t task);
    void (*yield)();
    void (*sleep)(uint32_t ticks);
    void (*endSelf)();
};

struct KAPI_t {
    KIOAPI_t kio;
    FIOAPI_t fio;
    MMAPI_t mm;
    VFSAPI_t vfs;
    MCTLAPI_t mctl;
    SchedulerAPI_t scheduler;
    IDTAPI_t idt;
    multiboot_info* boot_info;
};

namespace kapi {
    void init(multiboot_info_t* info);
    extern KAPI_t api;
}