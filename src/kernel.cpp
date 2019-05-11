#include <init/init.h>
#include <multiboot/multiboot.h>
#include <misc/liballoc.h>
#include <vfs/vfs.h>
#include <vfs/fileio.h>
#include <temp_vga/terminal.h>
#include <stream.h>
#include <vector.h>
#include <ramfs/tarfs.h>
#include <paging/paging.h>
#include <gdt/gdt.h>
#include <panic.h>
#include <syscalls.h>
#include <kernio/kernio.h>
#include <elf/elfldr.h>

struct VBEPMTable_t {
    uint16_t f5_offset;
    uint16_t f7_offset;
    uint16_t f9_offset;
};

void allocModule(multiboot_module_t mod) {
    paging::setPresent(mod.mod_start, ((mod.mod_end - mod.mod_start) / 4096) + 1);
    // TODO: Fix pages not being added
    
    uint32_t count = ((mod.mod_end - mod.mod_start) / 4096) + 1;
    for (int i = 0; i < count; i++) {
        //paging::setPresent(mod.mod_start + (i * 4096), 1);
    }
}

void listNodes(char* path) {
    Terminal.print("Listing ");
    Terminal.println(path);
    if (vfs::nodeExists(path)) {
        vector<FSNode_t> nodes = vfs::listNodes(path);
        for (int i = 0; i < nodes.size(); i++) {
            if (nodes[i].flags & FS_FLAG_D) {
                Terminal.setColor(0x09);
            }
            else {
                Terminal.setColor(0x07);
            }
            Terminal.print("> ");
            Terminal.println(nodes[i].name);
        }
        Terminal.println("");
        Terminal.setColor(0x0F);
    }
    else {
        Terminal.print("No such directory: ");
        Terminal.println(path);
    }
    
}

void printFile(char* path) {
    if (vfs::nodeExists(path)) {
        stream s = vfs::getStream(path);
        if (s.slen > 0) {
            Terminal.println("Printing file:");
            Terminal.println("--------------");
            char* buf = (char*)malloc(s.slen + 1);
            buf[s.slen] = 0;
            s.read(buf, s.slen);
            Terminal.print(buf);
        }
    }
    else {
        Terminal.print("No such file: ");
        Terminal.println(path);
    }
}

uint32_t _writeHndlr(stream* s, uint32_t len, uint64_t pos) {
    char* str = (char*)malloc(len + 1);
    memcpy(str, s->buffer, len);
    str[len] = 0;
    Terminal.print(str);
    free(str);
    return len;
}

uint32_t _readHndlr(stream* s, uint32_t len, uint64_t pos) {
    return 0;
}

void _closeHndlr(stream* s) {
    
}

extern "C"
void _kmain(uint32_t multiboot_magic, multiboot_info* multiboot_info) {
    _init(multiboot_info);
    kio::stdout = stream(0x1000, 10,_writeHndlr, _readHndlr, _closeHndlr, 0);

    kio::printf("THIS WAS PRINTED WITH STDOUT!!! %08x\n", 0xDEADBEEF);


    Terminal.println("LimeOS v1.2");

    vfs::init();
    Terminal.println("VFS driver initialized");

    fio::init();
    Terminal.println("Fileio driver initialized");

    kio::printf("\n\nUsed memory: %u bytes\n\n", paging::getUsedPages() * 4096);

    multiboot_module_t* mods = (multiboot_module_t*)multiboot_info->mods_addr;
    
    Terminal.println("Loading ramfs...");
    int id = -1;

    for (int i = 0; i < multiboot_info->mods_count; i++) {
        if (strcmp((char*)mods[i].cmdline, "ramfs")) {
            id = i;
        }
    }
    if (id == -1) {
        kpanic("Couldn't load RAMFS", 0);
    }
    allocModule(mods[id]);
    tarfs::init(mods[id].mod_start, "/");

    listNodes("/");
    listNodes("/bin");

    kio::printf("\n\nUsed memory: %u bytes\n\n", paging::getUsedPages() * 4096);

    kio::println("Loading elf...");
    stream s = vfs::getStream("/bin/i8080_tests.elf");
    kio::printf("ELF size: %u bytes\n", s.slen);
    char* buf = (char*)malloc(s.slen);
    s.read(buf, s.slen);
    s.close();

    ELFExec exec((uint32_t)buf);
    kio::printf("ELF magic: 0x%08x\n", exec.header->magic);

    vector<ELFSection_t> sect = exec.getSections();

    kio::println("Sections:");
    kio::printf("%u\n", sect.size());
    for (int i = 0; i < sect.size(); i++) {
        //kio::println(sect[i].name);
    }
    

    /*
    - drivers are loaded via a kernscript
    */

    while(1) {
        
    }
}