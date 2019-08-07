#pragma once
#include <stdint.h>
#include <string.h>
#include <multiboot/multiboot.h>
#include <elf/elfparser.h>

struct KModule_t {
    string name;
    string path;
    ELFFile_t code;
};

namespace kmod {
    void init(multiboot_info* boot_info);
    bool load(char* path);
    bool unload(char* name);
}