#include <multiboot/ksyms.h>
#include <elf/elf.h>

namespace ksyms {
    map<string, ELFSymbolTableEntry_t> symbols;

    void load(multiboot_info* multiboot_info) {
        symbols = map<string, ELFSymbolTableEntry_t>();
        ELFSectionHeaderEntry_t* sections = (ELFSectionHeaderEntry_t*)multiboot_info->u.elf_sec.addr;

        uint32_t symtab_id = 0;
        uint32_t symcount = 0;
        char* STStrTab = 0;
        for (int i = 0; i < multiboot_info->u.elf_sec.num; i++) {
            char* name = (char*)(sections[multiboot_info->u.elf_sec.shndx].addr + sections[i].name);
            if (sections[i].type == ELF_SHT_SYMTAB) {
                symtab_id = i;
                symcount = sections[i].size / sizeof(ELFSymbolTableEntry_t);
                STStrTab = (char*)(sections[sections[i].link].addr);
            }
        }

        symbols.reserve(symcount);      
        ELFSymbolTableEntry_t* syms = (ELFSymbolTableEntry_t*)sections[symtab_id].addr;
        for (int i = 0; i < symcount; i++) {
            ELFSymbolTableEntry_t sym;
            sym.addr = syms[i].addr;
            sym.info = syms[i].info;
            sym.name = syms[i].name;
            sym.other = syms[i].other;
            sym.sectionHeaderIndex = syms[i].sectionHeaderIndex;
            sym.size = syms[i].size;
            symbols.insert(string(&STStrTab[syms[i].name]), sym);
        }
    }
}