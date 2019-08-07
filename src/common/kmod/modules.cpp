#include <kmod/modules.h>
#include <map.h>
#include <paging/paging.h>
#include <kernio/kernio.h>
#include <vfs/vfs.h>
#include <multiboot/ksyms.h>
#include <elf/elfparser.h>

namespace kmod {
    map<string,KModule_t> modules;

    void init(multiboot_info* boot_info) {
        modules = map<string, KModule_t>();
    }

    bool load(char* path) {
        ELFFile_t mod = elfparser::load(path);

        // Load
        uint32_t length = 0;
        for (int i = 0; i < mod.programHeader.entryCount; i++) {
            if (mod.programHeader.entries[i].p_vaddr + mod.programHeader.entries[i].p_memsz > length && mod.programHeader.entries[i].type == ELF_PT_LOAD) {
                length = mod.programHeader.entries[i].p_vaddr + mod.programHeader.entries[i].p_memsz;
            }
        }
        uint32_t basePage = paging::allocPages(paging::sizeToPages(length));
        for (int i = 0; i < mod.programHeader.entryCount; i++) {
            if (mod.programHeader.entries[i].type == ELF_PT_LOAD) {
                memset((void*)(basePage + mod.programHeader.entries[i].p_vaddr), 0, mod.programHeader.entries[i].p_memsz);
                memcpy((void*)(basePage + mod.programHeader.entries[i].p_vaddr), &mod.data[mod.programHeader.entries[i].p_offset], mod.programHeader.entries[i].p_filesz);
            }
        }
        
        // Relocate
        for (int tbl = 0; tbl < mod.relocTables.size(); tbl++) {
            if (mod.relocTables[tbl].type == ELF_SHT_RELA) {
                kio::println("OH GOD OH FUCK, ITS NOT IMPLEMENTED!!!");
            }
            else {
                ELFSymbolTable_t symtab = mod.symbolTables[elfparser::getSymbolTableId(mod, mod.relocTables[tbl].symTab)];
                for (int i = 0; i < mod.relocTables[tbl].entryCount; i++) {
                    if (ELF_R_TYPE(mod.relocTables[tbl].relocs[i].info) == ELF_R_386_JMP_SLOT) {
                        ELFSymbolTableEntry_t sym = symtab.symbols[mod.relocTables[tbl].relocs[i].info >> 8];
                        if (!ksyms::symbols.exists(ELFGetString(symtab.stringTable, sym.name))) {
                            return false;
                        }
                        uint32_t* ptr = (uint32_t*)(mod.relocTables[tbl].relocs[i].addr + basePage);
                        ptr[0] = ksyms::symbols[ELFGetString(symtab.stringTable, sym.name)].addr;
                    }
                    if (ELF_R_TYPE(mod.relocTables[tbl].relocs[i].info) == ELF_R_386_GLOB_DAT) {
                        ELFSymbolTableEntry_t sym = symtab.symbols[mod.relocTables[tbl].relocs[i].info >> 8];
                        if (!ksyms::symbols.exists(ELFGetString(symtab.stringTable, sym.name))) {
                            return false;
                        }
                        uint32_t* ptr = (uint32_t*)(mod.relocTables[tbl].relocs[i].addr + basePage);
                        ptr[0] = ksyms::symbols[ELFGetString(symtab.stringTable, sym.name)].addr;
                    }
                    else if (ELF_R_TYPE(mod.relocTables[tbl].relocs[i].info) == ELF_R_386_RELATIVE) {
                        uint32_t* ptr = (uint32_t*)(mod.relocTables[tbl].relocs[i].addr + basePage);
                        ptr[0] = mod.offset + ptr[0];
                    }
                }
            }
        }

        // TODO: Might need to call array init and shit

        // Call
        bool (*func_ptr)() = (bool (*)())(mod.header->progEntryPos + basePage);
        bool ret = func_ptr();

        // TODO, free memory
        return ret;
    }
    
    bool unload(char* name) {
        return false;
    }
}