#include <elf/elfldr.h>
#include <elf/elf.h>
#include <kernio/kernio.h>
#include <vfs/vfs.h>
#include <paging/paging.h>

ELFExec::ELFExec(uint32_t addr) {
    data = (char*)addr;
    header = (ELFHeader_t*)addr;
    sectionHeader = (ELFSectionHeader_t*)(header->sectionHeaderTablePos + addr);
    SHEntryCount = header->sectionHeaderTableEntryCount;
    SHStringTable = (char*)(sectionHeader[header->sectionNameIndex].offset + addr);
    // Search symbol table
    for (int i = 0; i < SHEntryCount; i++) {
        if (sectionHeader[i].type == ELF_SHT_SYMTAB) {
            symbolTable = (ELFSymbolTableEntry_t*)(addr + sectionHeader[i].offset);
            STStringTable = (char*)(addr + sectionHeader[sectionHeader[i].link].offset);
            STEntryCount = sectionHeader[i].size / sizeof(ELFSymbolTableEntry_t);
        }
    }
}

vector<ELFSection_t> ELFExec::getSections() {
    vector<ELFSection_t> v;
    for (int i = 0; i < SHEntryCount; i++) {
        ELFSection_t s;
        s.name = &SHStringTable[sectionHeader[i].name];
        s.addr = sectionHeader[i].addr;
        s.size = sectionHeader[i].size;
        s.type = sectionHeader[i].type;
        s.flags = sectionHeader[i].flags;
        s.offset = sectionHeader[i].offset;
        s.data = (char*)((uint32_t)data + sectionHeader[i].offset);
        s.link = sectionHeader[i].link;
        s.info = sectionHeader[i].info;
        s.align = sectionHeader[i].align;
        s.entrySize = sectionHeader[i].entrySize;
        v.push_back(s);
    }
    return v;
}

vector<ELFSymbol_t> ELFExec::getSymbols() {
    vector<ELFSymbol_t> v;
    for (int i = 0; i < STEntryCount; i++) {
        ELFSymbol_t s;
        s.name = &STStringTable[symbolTable[i].name];
        s.addr = symbolTable[i].addr;
        s.size = symbolTable[i].size;
        s.info = symbolTable[i].info;
        s.other = symbolTable[i].other;
        s.sectionHeaderIndex = symbolTable[i].sectionHeaderIndex;
        v.push_back(s);
    }
    return v;
}

namespace elfldr {
    void run(char* path){
        stream s = vfs::getStream(path);
        char* buf = (char*)malloc(s.slen);
        s.read(buf, s.slen);
        s.close();
        ELFExec exec((uint32_t)buf);

        uint32_t end = 0;
        vector<ELFSection_t> sect = exec.getSections();
        for (int i = 0; i < sect.size(); i++) {
            if (sect[i].addr + sect[i].size > end) {
                end = sect[i].addr + sect[i].size;
            }
        }

        // Allocate pages
        char* loadAddr = (char*)paging::allocPages((end / 4096) + 1);

        // Load sections
        for (int i = 0; i < sect.size(); i++) {
            if (sect[i].flags & ELF_SHF_ALLOC) {
                memcpy(sect[i].addr + loadAddr, sect[i].data, sect[i].size);
            }
        }

        uint32_t entryPtr = 0;

        vector<ELFSymbol_t> syms = exec.getSymbols();
        for (int i = 0; i < syms.size(); i++) {
            if (strcmp(syms[i].name, "_start")) {
                entryPtr = syms[i].addr + (uint32_t)loadAddr;
            }
        }

        if (entryPtr == 0) {
            kio::println("NO ENTRY POINT FOUND!!!");
            return;
        }

        void (*func_ptr)() = (void (*)())(entryPtr);

        func_ptr();

        kio::println("<== Returned ==>");
    }
}