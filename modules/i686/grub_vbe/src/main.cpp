#include <misc/memory.h>
#include <string.h>
#include <stdctl/framebuffer.h>
#include <stream.h>
#include <multiboot/multiboot.h>
#include <vfs/fileio.h>
#include <vfs/vfs.h>
#include <paging/paging.h>
#include <kernio/kernio.h>
#include <kmod/mctl.h>

char* fb;

uint32_t _writeHndlr(stream_t s, uint32_t len, uint64_t pos) {
    memcpy(fb + pos, s.buffer, len);
    return len;
}

uint32_t _readHndlr(stream_t s, uint32_t len, uint64_t pos) {
    return 0;
}

void _closeHndlr(stream_t s) {
    
}

stream_t _provider(void* tag) {
    uint32_t length = (msb::boot_info->framebuffer_bpp / 8) * msb::boot_info->framebuffer_width * msb::boot_info->framebuffer_height;
    return stream::create(0x1000, length, _writeHndlr, _readHndlr, _closeHndlr, 0);;
}

char* genDevName() {
    vector<FSNode_t> nodes = vfs::listNodes("/fio/dev");
    uint32_t count = 0;
    for (int i = 0; i < nodes.size(); i++) {
        if (strsw(nodes[i].name, "fb") && nodes[i].name[3] >= '0' && nodes[i].name[3] <= '9') {
            count++;
        }
    }
    string num = itoa(count, 10);
    char* base = "/dev/fb";
    char* name = (char*)malloc(8 + num.length() + 1);
    memcpy(name, base, 7);
    memcpy(name + 7, num.c_str(), num.length());
    name[7 + num.length()] = 0;
    return name;
}

FramebufferInfo_t fbInfo;

int _mctlHndlr(void* tag, uint32_t id, void* in, void* out) {
    if (id == 1) { // FB_MCTL_CMD_GETINFO
        memcpy(out, &fbInfo, sizeof(fbInfo));
        return sizeof(FramebufferInfo_t);
    }
    else {
        return -1;
    }
}

extern "C"
bool _start() {
    kio::println("[grub_vbe] Initializing frame buffer...");
    uint32_t length = (msb::boot_info->framebuffer_bpp / 8) * msb::boot_info->framebuffer_width * msb::boot_info->framebuffer_height;
    paging::setPresent(msb::boot_info->framebuffer_addr, paging::sizeToPages(length));
    kio::println("[grub_vbe] PAGES ARE NOW PRESENT, CLEARING...");
    fb = (char*)msb::boot_info->framebuffer_addr;
    for (int i = 0; i < length; i++) {
        fb[i] = 0;
    }
    fbInfo.addr = msb::boot_info->framebuffer_addr;
    fbInfo.width = msb::boot_info->framebuffer_width;
    fbInfo.height = msb::boot_info->framebuffer_height;
    fbInfo.bpp = msb::boot_info->framebuffer_bpp;
    fbInfo.pitch = msb::boot_info->framebuffer_pitch;

    char* name = genDevName();
    kio::print("[grub_vbe] Mounting ");
    kio::print(name);
    kio::println(" ...");

    fio::mountStreamProvider(name, 0, _provider, NULL);

    MCTLHandler_t mctlHandler;
    mctlHandler.tag = 0;
    mctlHandler._handler = _mctlHndlr;
    mctl::registerHndlr(name, mctlHandler);

    kio::println("[grub_vbe] Done.");
    return true;
}