#pragma once
#include <stdint.h>
#include <elf/elf.h>
#include <vector.h>

#define ELFGetString(strTab, index) &strTab[index]

struct ELFSectionHeader_t {
    uint32_t entryCount;
    char* stringTable;
    ELFSectionHeaderEntry_t* sections;
};

struct ELFSymbolTable_t {
    uint32_t sectionIndex;
    uint32_t entryCount;
    char* stringTable;
    char* name;
    uint8_t type;
    ELFSymbolTableEntry_t* symbols;
};

struct ELFRelocTable_t {
    uint32_t sectionIndex;
    uint8_t type;
    uint32_t entryCount;
    uint32_t affected;
    uint32_t symTab;
    union {
        ELFReloc_t* relocs;
        ELFRelocAddend_t* addendRelocs;
    };
};

struct ELFProgramHeader_t {
    uint32_t entryCount;
    ELFProgramHeaderEntry_t* entries;
};

struct ELFFile_t {
    char* data;
    uint32_t offset;
    ELFHeader_t* header;
    ELFSectionHeader_t sectionHeader;
    ELFProgramHeader_t programHeader;
    vector<ELFSymbolTable_t> symbolTables;
    vector<ELFRelocTable_t> relocTables;
    uint64_t init = 0;

    ELFFile_t() {
        symbolTables = vector<ELFSymbolTable_t>();
        relocTables = vector<ELFRelocTable_t>();
    }

    ELFFile_t(const ELFFile_t& e) {
        data = e.data;
        header = e.header;
        sectionHeader = e.sectionHeader;
        symbolTables = e.symbolTables;
        relocTables = e.relocTables;
    }

    ELFFile_t& operator=(ELFFile_t& e) {
        data = e.data;
        header = e.header;
        sectionHeader = e.sectionHeader;
        symbolTables = e.symbolTables;
        relocTables = e.relocTables;
        return *this;
    }

    ~ELFFile_t() {
        // TODO: get this to work to avoid omega memory leak
    }
};

namespace elfparser {
    ELFFile_t load(char* path);
    int getSymbolTableId(ELFFile_t& file, int sectionId);
}