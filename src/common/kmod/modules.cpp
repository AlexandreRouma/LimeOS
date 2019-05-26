#include <kmod/modules.h>
#include <elf/elfldr.h>
#include <map.h>
#include <paging/paging.h>
#include <kernio/kernio.h>
#include <vfs/vfs.h>
#include <kapi/api/kapi.h>
#include <multiboot/ksyms.h>

namespace kmod {
    map<string,KModule_t> modules;

    void init(multiboot_info* boot_info) {
        modules = map<string, KModule_t>();
        kapi::init(boot_info);
    }

    bool load(char* path) {
        stream_t s = vfs::getStream(path);
        uint32_t pages = paging::sizeToPages(s.slen);
        char* buf = (char*)paging::allocPages(pages);
        stream::read(s, buf, s.slen);
        stream::close(s);
        ELFExec mod((uint32_t)buf);

        uint32_t end = 0;
        vector<ELFSection_t> sect = mod.getSections();
        for (int i = 0; i < sect.size(); i++) {
            if (sect[i].addr + sect[i].size > end) {
                end = sect[i].addr + sect[i].size;
            }
        }

        // Allocate pages
        char* loadAddr = (char*)paging::allocPages(paging::sizeToPages(end));

        // Load sections
        for (int i = 0; i < sect.size(); i++) {
            if (sect[i].flags & ELF_SHF_ALLOC) {
                memcpy(sect[i].addr + loadAddr, sect[i].data, sect[i].size);
            }
        }

        uint32_t entryPtr = 0;

        vector<ELFSymbol_t> syms = mod.getSymbols();
        for (int i = 0; i < syms.size(); i++) {
            if (strcmp(syms[i].name, "_start")) {
                entryPtr = syms[i].addr + (uint32_t)loadAddr;
            }
        }

        if (entryPtr == 0) {
            return false;
        }

        bool (*func_ptr)(KAPI_t) = (bool (*)(KAPI_t))(entryPtr);
        
        bool ret = func_ptr(kapi::api);
        if (ret == true) {
            paging::setAbsent((uint32_t)buf, pages);
            // TODO: Save module
        }
        return ret;
    }

    bool loadDyn(char* path, multiboot_info* multiboot_info) {
        kio::println("Loading module...");
        stream_t s = vfs::getStream(path);
        uint32_t pages = paging::sizeToPages(s.slen);
        char* buf = (char*)paging::allocPages(pages);
        stream::read(s, buf, s.slen);
        stream::close(s);
        ELFExec mod((uint32_t)buf);

        kio::println("Loading sections...");
        uint32_t end = 0;
        vector<ELFSection_t> sect = mod.getSections();
        for (int i = 0; i < sect.size(); i++) {
            if (sect[i].addr + sect[i].size > end) {
                end = sect[i].addr + sect[i].size;
            }
        }

        // Allocate pages
        char* loadAddr = (char*)paging::allocPages(paging::sizeToPages(end));

        // Load sections
        kio::println("Copying sections...");
        for (int i = 0; i < sect.size(); i++) {
            if (sect[i].flags & ELF_SHF_ALLOC) {
                memcpy(sect[i].addr + loadAddr, sect[i].data, sect[i].size);
            }
        }

        uint32_t entryPtr = 0;

        kio::println("Loading symbols...");
        vector<ELFSymbol_t> syms = mod.getSymbols(true);
        for (int i = 0; i < syms.size(); i++) {
            kio::println(syms[i].name);
            if (strcmp(syms[i].name, "_start")) {
                entryPtr = syms[i].addr + (uint32_t)loadAddr;
            }
        }

        if (entryPtr == 0) {
            kio::println("NO ENTRY POINT FOUND !!!!!!...");
            return false;
        }

        // PATCH GOT

        vector<ELFRelocate_t> relocs = mod.getRelocs();
        kio::printf("Reloc count: %u\n", relocs.size());
        for (int i = 0; i < relocs.size(); i++) {
            kio::printf("Symbol index: 0x%08X,0x%08X\n", relocs[i].info >> 8, relocs[i].addr);
            ELFSymbol_t sym = syms[relocs[i].info >> 8];
            if (!ksyms::symbols.exists(sym.name)) {
                kio::print("ERROR: Missing symbole: ");
                kio::println(sym.name);
                return false;
            }
            kio::print("Relocating: ");
            kio::println(sym.name);
            uint32_t* ptr = (uint32_t*)(relocs[i].addr + (uint32_t)loadAddr);
            ptr[0] = ksyms::symbols[sym.name].addr;
        }

        // PATCH GOT

        bool (*func_ptr)(KAPI_t) = (bool (*)(KAPI_t))(entryPtr);
        
        bool ret = func_ptr(kapi::api);
        if (ret == true) {
            paging::setAbsent((uint32_t)buf, pages);
            // TODO: Save module
        }
        return ret;
    }
    
    bool unload(char* name) {
        return false;
    }
}