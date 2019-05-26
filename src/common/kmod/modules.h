#pragma once
#include <stdint.h>
#include <elf/elfldr.h>
#include <string.h>
#include <multiboot/multiboot.h>

struct KModule_t {
    string name;
    string path;
    ELFExec code;
};

namespace kmod {
    void init(multiboot_info* boot_info);
    bool load(char* path);
    bool loadDyn(char* path, multiboot_info* multiboot_info);
    bool unload(char* name);
}