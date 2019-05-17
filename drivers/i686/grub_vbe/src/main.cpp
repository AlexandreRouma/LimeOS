#include <kapi.h>
#include <memory.h>
#include <string.h>

char* fb;

uint32_t _writeHndlr(stream_t s, uint32_t len, uint64_t pos) {
    memcpy(fb + pos, s.buffer, len);
    return len;
}

uint32_t _readHndlr(stream_t s, uint32_t len, uint64_t pos) {
    return 0;
}

void _closeHndlr(stream_t s) {
    if (s.buffer != 0) {
        kapi::api.mm.free(s.buffer);
        s.buffer = 0;
    }
}

stream_t _provider() {
    uint32_t length = (kapi::api.boot_info->framebuffer_bpp / 8) * kapi::api.boot_info->framebuffer_width * kapi::api.boot_info->framebuffer_height;
    return stream::create(0x1000, length, _writeHndlr, _readHndlr, _closeHndlr, 0);;
}

char* genDevName() {
    vector<FSNode_t> nodes = kapi::api.vfs.listNodes("/fio/dev");
    uint32_t count = 0;
    for (int i = 0; i < nodes.size(); i++) {
        if (strsw(nodes[i].name, "fb") && nodes[i].name[3] >= '0' && nodes[i].name[3] <= '9') {
            count++;
        }
    }
    string num = itoa(count, 10);
    char* base = "/dev/fb";
    char* name = (char*)kapi::api.mm.malloc(8 + num.length() + 1);
    memcpy(name, base, 7);
    memcpy(name + 7, num.c_str(), num.length());
    name[7 + num.length()] = 0;
    return name;
}

extern "C"
bool _start(KAPI_t api) {
    kapi::api = api;
    api.kio.println("[grub_vbe] Initializing frame buffer...");
    uint32_t length = (api.boot_info->framebuffer_bpp / 8) * api.boot_info->framebuffer_width * api.boot_info->framebuffer_height;
    api.mm.setPresent(api.boot_info->framebuffer_addr, (length / 4096) + 1);
    fb = (char*)api.boot_info->framebuffer_addr;
    for (int i = 0; i < length; i++) {
        fb[i] = 0;
    }

    char* name = genDevName();
    api.kio.print("[grub_vbe] Mounting ");
    api.kio.print(name);
    api.kio.println(" ...");

    api.fio.mountStreamProvider("/dev/fb0", 0, _provider);
    api.kio.println("[grub_vbe] Done.");
    return true;
}