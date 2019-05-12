#pragma once
#include <stdint.h>
#include <elf/elf.h>
#include <vector.h>

struct ELFSection_t{
    char* name;
    uint32_t type;
    uint32_t flags;
    uint32_t addr;
    uint32_t offset;
    char* data;
    uint32_t size;
    uint32_t link;
    uint32_t info;
    uint32_t align;
    uint32_t entrySize;
};

struct ELFSymbol_t{
    char* name;
    uint32_t addr;
    uint32_t size;
    uint8_t info;
    uint8_t other;
    uint16_t sectionHeaderIndex;
};

class ELFExec {
public:
    ELFExec(uint32_t addr);
    vector<ELFSection_t> getSections();
    vector<ELFSymbol_t> getSymbols();

    char* data;
    ELFHeader_t* header;
    uint32_t SHEntryCount;
    ELFSectionHeader_t* sectionHeader;
    uint32_t STEntryCount;
    ELFSymbolTableEntry_t* symbolTable;
    char* SHStringTable;
    char* STStringTable;

private:
    uint32_t _addr;
};

namespace elfldr {
    void run(char* path);
}