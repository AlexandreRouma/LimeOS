#pragma once
#include <multiboot/multiboot.h>
#include <map.h>
#include <string.h>
#include <elf/elfldr.h>

namespace ksyms {
    void load(multiboot_info* multiboot_info);
    extern map<string, ELFSymbol_t> symbols;
}