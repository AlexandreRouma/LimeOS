#pragma once
#include <stdint.h>
#include <elf/elf.h>
#include <vector.h>

struct ELFSection_t{
    char* name;
    uint32_t addr;
};

struct ELFSymbol_t{
    char* name;
    uint32_t addr;
};


namespace elfldr {

}

class ELFExec {
public:
    ELFExec(uint32_t addr);
    vector<ELFSection_t> getSections();
    
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