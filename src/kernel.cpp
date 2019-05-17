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

    listNodes("/fio/dev");

    kio::printf("1) Using %u bytes\n", paging::getUsedPages() * 4096);



    // stream_t framebuffer = vfs::getStream("/fio/dev/fb0");
    // stream_t image = vfs::getStream("/misc/lime2.bmp");
    // stream_t font = vfs::getStream("/misc/font.bmp");

    // uint32_t bitmapAlloc = paging::sizeToPages(image.slen);
    // uint32_t fontAlloc = paging::sizeToPages(font.slen);
    // uint32_t screenAlloc = paging::sizeToPages(1920 * 1080 * 4);

    // uint8_t* bmp = (uint8_t*)paging::allocPages(bitmapAlloc);
    // uint8_t* fnt = (uint8_t*)paging::allocPages(fontAlloc);
    // uint8_t* screen = (uint8_t*)paging::allocPages(screenAlloc);
    // stream::read(image, (char*)bmp, image.slen);
    // stream::close(image);

    // stream::read(font, (char*)fnt, font.slen);
    // stream::close(font);

    // uint8_t* data = bmp + 0x36;
    // uint8_t* fData = fnt + 0x36;

    // for (int y = 0; y < 1080; y++) {
    //     for (int x = 0; x < 1920; x++) {
    //         uint32_t screenOffset = ((y * 1920) + x) * 4;
    //         uint32_t imgOffset = ((y * 1920) + x) * 3;
    //         uint32_t fntOffset = (((255 - y) * 256) + x) * 3;
    //         if (x >= 256 || y >= 256) {
    //             fntOffset = 0;
    //         }

    //         uint8_t r = 0;
    //         uint8_t g = 0;
    //         uint8_t b = 0;

    //         if (y < 16 && y > 0) {
    //             r = 255;
    //             g = 0;
    //             b = 0;
    //         }
    //         else if (y < 32 && y > 16) {
    //             r = 255;
    //             g = 127;
    //             b = 0;
    //         }
    //         else if (y < 48 && y > 32) {
    //             r = 255;
    //             g = 255;
    //             b = 0;
    //         }
    //         else if (y < 56 && y > 48) {
    //             r = 127;
    //             g = 255;
    //             b = 0;
    //         }
    //         else if (y < 64 && y > 56) {
    //             r = 0;
    //             g = 255;
    //             b = 0;
    //         }
    //         else if (y < 80 && y > 64) {
    //             r = 0;
    //             g = 255;
    //             b = 127;
    //         }
    //         else if (y < 96 && y > 80) {
    //             r = 0;
    //             g = 255;
    //             b = 255;
    //         }
    //         else if (y < 112 && y > 96) {
    //             r = 0;
    //             g = 0;
    //             b = 255;
    //         }
    //         else {
    //             r = 255;
    //             g = 255;
    //             b = 255;
    //         }

    //         float fCoef = (float)fData[fntOffset] / 255.0f;
    //         float bCoef = (1.0f - fCoef) / 6;
    //         screen[screenOffset] = (data[imgOffset + 0] * bCoef) + (fCoef * b);
    //         screen[screenOffset + 1] = (data[imgOffset + 1] * bCoef) + (fCoef * g);
    //         screen[screenOffset + 2] = (data[imgOffset + 2] * bCoef) + (fCoef * r);
    //     }
    // }

    // stream::write(framebuffer, (char*)screen, 1920 * 1080 * 4);

    // paging::setAbsent((uint32_t)bmp, bitmapAlloc);
    // paging::setAbsent((uint32_t)screen, screenAlloc);
    // stream::close(framebuffer);

    kio::printf("2) Using %u bytes\n", paging::getUsedPages() * 4096);
    

    /*
    - drivers are loaded via a kernscript
    */

    while(1) {
        
    }
}