#pragma once
#include <stdint.h>
#include <stdarg.h>
#include <stream.h>

#ifdef KERNEL_CODE
#include <multiboot/multiboot.h>
#else
#include <multiboot.h>
#endif

struct FIOAPI_t {
    bool (*createDir)(char*);
    bool (*mountStreamProvider)(char*, uint16_t, stream_t (*)());
};

struct KIOAPI_t {
    void (*print)(char*);
    void (*println)(char*);
};

struct MMAPI_t {
    void* (*malloc)(uint32_t size);
    void (*free)(void*);
    void* (*memcpy)(void*, void*, uint64_t);
    void* (*realloc)(void*, uint32_t);
};

struct KAPI_t {
    KIOAPI_t kio;
    FIOAPI_t fio;
    MMAPI_t mm;
    multiboot_info* boot_info;
};

namespace kapi {
    void init(multiboot_info_t* info);
    extern KAPI_t api;
}