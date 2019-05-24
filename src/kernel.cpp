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
#include <kmod/mctl.h>
#include <misc/cpuio.h>
#include <scheduler/scheduler.h>

#define BochsBreak() outw(0x8A00,0x8A00); outw(0x8A00,0x08AE0);

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

int id = 2;

void testTask2() {
    while (1) {
        kio::print("Task 2\n");
        scheduler::sleep(576);
    }
}

void testTask3() {
    while (1) {
        kio::print("Task 3\n");
        scheduler::sleep(689);
    }
}

void testTask4() {
    while (1) {
        kio::print("Task 4\n");
        scheduler::sleep(731);
    }
}

void testTask5() {
    while (1) {
        kio::print("Task 5\n");
        scheduler::sleep(597);
    }
}

void testTask6() {
    while (1) {
        kio::print("Task 6\n");
        scheduler::sleep(832);
    }
}

void testTask7() {
    while (1) {
        kio::print("Task 7\n");
        scheduler::sleep(477);
    }
}

void testTask8() {
    while (1) {
        kio::print("Task 8\n");
        scheduler::sleep(621);
    }
}

void testTask9() {
    while (1) {
        kio::print("Task 9\n");
        scheduler::sleep(666);
    }
}

void testTask10() {
    while (1) {
        kio::print("Task 10\n");
        scheduler::sleep(420);
    }
}

extern "C"
void _kmain(uint32_t multiboot_magic, multiboot_info* multiboot_info) {
    _init(multiboot_info);

    // Protect modules
    multiboot_module_t* mods = (multiboot_module_t*)multiboot_info->mods_addr;
    for (int i = 0; i < multiboot_info->mods_count; i++) {
        allocModule(mods[i]);
    }

    // Init VFS
    vfs::init();

    // Init fileio
    fio::init();
    miscstrms::init();

    // Module Control
    mctl::init();

    // Redirect kernel stdout
    kio::stdout = vfs::getStream("/fio/dev/null");

    // Load ram filesystem
    int id = -1;

    for (int i = 0; i < multiboot_info->mods_count; i++) {
        if (strcmp((char*)mods[i].cmdline, "ramfs")) {
            id = i;
        }
    }
    if (id == -1) {
        kpanic("Couldn't load RAMFS", 0);
    }
    tarfs::init(mods[id].mod_start, "/");

    // Init module interface
    kmod::init(multiboot_info);

    // run kscript
    ksc::init();
    ksc::run("/conf/init.ksc");

    listNodes("/");
    listNodes("/fio");
    listNodes("/fio/dev");

    kio::printf("1) Using %u bytes\n", paging::getUsedPages() * 4096);

    kio::printf("2) Using %u bytes\n", paging::getUsedPages() * 4096);

    kio::println("");

    kio::setFore(10);
    kio::print("urmom@fakeshell");
    kio::setFore(15);
    kio::print(":");
    kio::setFore(12);
    kio::print("/fio/dev");
    kio::setFore(15);
    kio::print("$ ls");

    //BochsBreak();

    for (int i = 10; i > 0; i--) {
        kio::printf("Please wait %u seconds...\n", i);
        scheduler::sleep(1000);
    }

    uint32_t stackPage = 0;

    stackPage = paging::allocPages(1) + 0x800; // Get page top
    scheduler::createTask((uint32_t)testTask2, stackPage, 4096, 0, 0);

    stackPage = paging::allocPages(1) + 0x800; // Get page top
    scheduler::createTask((uint32_t)testTask3, stackPage, 4096, 0, 0);

    stackPage = paging::allocPages(1) + 0x800; // Get page top
    scheduler::createTask((uint32_t)testTask4, stackPage, 4096, 0, 0);

    stackPage = paging::allocPages(1) + 0x800; // Get page top
    scheduler::createTask((uint32_t)testTask5, stackPage, 4096, 0, 0);

    stackPage = paging::allocPages(1) + 0x800; // Get page top
    scheduler::createTask((uint32_t)testTask6, stackPage, 4096, 0, 0);

    stackPage = paging::allocPages(1) + 0x800; // Get page top
    scheduler::createTask((uint32_t)testTask7, stackPage, 4096, 0, 0);

    stackPage = paging::allocPages(1) + 0x800; // Get page top
    scheduler::createTask((uint32_t)testTask8, stackPage, 4096, 0, 0);

    stackPage = paging::allocPages(1) + 0x800; // Get page top
    scheduler::createTask((uint32_t)testTask9, stackPage, 4096, 0, 0);

    stackPage = paging::allocPages(1) + 0x800; // Get page top
    scheduler::createTask((uint32_t)testTask10, stackPage, 4096, 0, 0);

    /*
    - drivers are loaded via a kernscript
    */

   // NOTE: This loop is for the scheduler to fall back to when no task is currently running
    while(1) {
        kio::printf("Task 1 <======== TEST ========>\n");
        //scheduler::yield();
        scheduler::sleep(1000);
    }
}