#pragma once
#include <stdint.h>
#include <elf/elfldr.h>
#include <string.h>

struct KModule_t {
    string name;
    string path;
    ELFExec code;
};

namespace kmod {
    void init();
    bool load(char* path);
    bool unload(char* name);
}