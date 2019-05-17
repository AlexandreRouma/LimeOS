#include <kapi.h>
#include <memory.h>
#include <string.h>
#include <cpuio.h>

uint32_t _writeHndlr(stream_t s, uint32_t len, uint64_t pos) {
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
    return stream::create(0x1000, 0, _writeHndlr, _readHndlr, _closeHndlr, 0);;
}

char* genDevName() {
    vector<FSNode_t> nodes = kapi::api.vfs.listNodes("/fio/dev");
    uint32_t count = 0;
    for (int i = 0; i < nodes.size(); i++) {
        if (strsw(nodes[i].name, "tty") && nodes[i].name[3] >= '0' && nodes[i].name[3] <= '9') {
            count++;
        }
    }
    string num = itoa(count, 10);
    char* base = "/dev/tty";
    char* name = (char*)kapi::api.mm.malloc(8 + num.length() + 1);
    memcpy(name, base, 8);
    memcpy(name + 8, num.c_str(), num.length());
    name[8 + num.length()] = 0;
    return name;
}

char* background;

void loadBgImg(char* path) {
    stream_t img_stream = kapi::api.vfs.getStream("/fio/dev/fb0");
    char* buffer = (char*)kapi::api.mm.malloc(img_stream.slen);
    stream::read(img_stream, buffer, img_stream.slen);
    stream::close(img_stream);

    background = (char*)kapi::api.mm.malloc(1280 * 720 * 4);

    // TODO: Switch to direct buffer instead of stream, get proper fb info, add different files for different fb types.
}

extern "C"
bool _start(KAPI_t api) {
    kapi::api = api;
    api.kio.println("[gfx_term] Initializing graphic terminal...");

    if (!api.vfs.nodeExists("/fio/dev/fb0")) {
        api.kio.println("[gfx_term] ERROR: No framebuffer device found!");
        return false;
    }

    stream_t fb_stream = api.vfs.getStream("/fio/dev/fb0");

    char* buf = (char*)api.mm.malloc(fb_stream.slen);

    for (uint32_t i = 0; i < fb_stream.slen; i++) {
        buf[i] = 0xFF;
    }

    stream::write(fb_stream, buf, fb_stream.slen);

    char* name = genDevName();
    api.kio.print("[gfx_term] Mounting ");
    api.kio.print(name);
    api.kio.println(" ...");

    api.fio.mountStreamProvider(name, FS_FLAG_O_W | FS_FLAG_O_R, _provider);
    api.kio.println("[gfx_term] Done.");
    return true;
}