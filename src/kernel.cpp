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
#include <vfs/miscstreams.h>
#include <kscript/kscript.h>
#include <kmod/modules.h>

struct VBEPMTable_t {
    uint16_t f5_offset;
    uint16_t f7_offset;
    uint16_t f9_offset;
};

void allocModule(multiboot_module_t mod) {
    paging::setPresent(mod.mod_start, ((mod.mod_end - mod.mod_start) / 4096) + 1);
}

void listNodes(char* path) {
    kio::print("Listing ");
    kio::println(path);
    if (vfs::nodeExists(path)) {
        vector<FSNode_t> nodes = vfs::listNodes(path);
        for (int i = 0; i < nodes.size(); i++) {
            if (nodes[i].flags & FS_FLAG_D) {
                kio::setFore(0x09);
            }
            else {
                kio::setFore(0x07);
            }
            kio::print("> ");
            kio::println(nodes[i].name);
        }
        kio::println("");
        kio::setFore(0x0F);
    }
    else {
        kio::print("No such directory: ");
        kio::println(path);
    }
    
}

void printFile(char* path) {
    if (vfs::nodeExists(path)) {
        stream_t s = vfs::getStream(path);
        if (s.slen > 0) {
            kio::println("Printing file:");
            kio::println("--------------");
            char* buf = (char*)malloc(s.slen + 1);
            buf[s.slen] = 0;
            stream::read(s, buf, s.slen);
            kio::print(buf);
        }
    }
    else {
        kio::print("No such file: ");
        kio::println(path);
    }
}

uint32_t _writeHndlr(stream_t s, uint32_t len, uint64_t pos) {
    char* buf = (char*)malloc(len + 1);
    memcpy(buf, s.buffer, len);
    buf[len] = 0;
    Terminal.print(buf);
    free(buf);
    return len;
}

uint32_t _readHndlr(stream_t s, uint32_t len, uint64_t pos) {
    return 0;
}

void _closeHndlr(stream_t s) {
    
}

stream_t teststream;

stream_t provider() {
    return teststream;
}

extern "C"
void _kmain(uint32_t multiboot_magic, multiboot_info* multiboot_info) {
    _init(multiboot_info);

    // Init VFS
    vfs::init();

    // Init fileio
    fio::init();
    miscstrms::init();

    // Init term driver
    teststream = stream::create(0x1000, 0, _writeHndlr, _readHndlr, _closeHndlr, NULL);
    fio::mountStreamProvider("/dev/tty1", FS_FLAG_O_W, provider);

    // Redirect kernel stdout
    kio::stdout = vfs::getStream("/fio/dev/tty1");

    // Load ram filesystem
    multiboot_module_t* mods = (multiboot_module_t*)multiboot_info->mods_addr;
    kio::println("Loading ramfs...");
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

    // Init module interface
    kmod::init(multiboot_info);

    listNodes("/fio/dev");

    // run kscript
    ksc::init();
    ksc::run("/conf/init.ksc");

    listNodes("/fio/dev");

    /*
    - drivers are loaded via a kernscript
    */

    while(1) {
        
    }
}