#include <kapi.h>
#include <memory.h>
#include <string.h>
#include <cpuio.h>
#include <stdctl/framebuffer.h>

int TERM_WIDTH = 0;
int TERM_HEIGHT = 0;

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

uint8_t* background;
uint8_t* font;
FramebufferInfo_t fbInfo;

void loadBgImg(char* path) {
    stream_t img_stream = kapi::api.vfs.getStream(path);
    char* buffer = (char*)kapi::api.mm.malloc(img_stream.slen);
    stream::read(img_stream, buffer, img_stream.slen);
    stream::close(img_stream);

    kapi::api.kio.println("File loaded!");

    uint8_t* data = (uint8_t*)&buffer[0x36];

    for (int y = 0; y < fbInfo.height; y++) {
        for (int x = 0; x < fbInfo.width; x++) {
            background[((y * fbInfo.width) + x) * 4] = (float)data[((y * 1920) + x) * 3] * 0.2f;
            background[((y * fbInfo.width) + x) * 4 + 1] = (float)data[((y * 1920) + x) * 3 + 1] * 0.2f;
            background[((y * fbInfo.width) + x) * 4 + 2] = (float)data[((y * 1920) + x) * 3 + 2] * 0.2f;
        }
    }
    kapi::api.mm.free(buffer);
    // TODO: Switch to direct buffer instead of stream, get proper fb info, add different files for different fb types.
}

void drawBackground() {
    memcpy((char*)fbInfo.addr, background, fbInfo.height * fbInfo.pitch);
}

void drawChar(char c, int x, int y) {
    uint8_t* fb = (uint8_t*)fbInfo.addr;
    uint8_t* fd = &font[(c * 256)];
    for (int sy = 0; sy < 16; sy++) {
        for (int sx = 0; sx < 16; sx++) {
            float fg = (float)fd[(sy * 16) + sx] / 256.0f;
            float bg = 1.0f - fg;
            fb[(((sy + y) * fbInfo.width) + (sx + x)) * 4] = (fg * 255) + (bg * background[(((sy + y) * fbInfo.width) + (sx + x)) * 4]);
            fb[(((sy + y) * fbInfo.width) + (sx + x)) * 4 + 1] = (fg * 255) + (bg * background[(((sy + y) * fbInfo.width) + (sx + x)) * 4 + 1]);
            fb[(((sy + y) * fbInfo.width) + (sx + x)) * 4 + 2] = (fg * 255) + (bg * background[(((sy + y) * fbInfo.width) + (sx + x)) * 4 + 2]);
        }
    }
}

extern "C"
bool _start(KAPI_t api) {
    kapi::api = api;
    api.kio.println("[gfx_term] Initializing graphic terminal...");

    if (!api.vfs.nodeExists("/fio/dev/fb0")) {
        api.kio.println("[gfx_term] ERROR: No framebuffer device found!");
        return false;
    }

    int ret = api.mctl.call("/dev/fb0", FB_MCTL_CMD_GETINFO, 0, &fbInfo);
    if (ret <= 0) {
        api.kio.println("[gfx_term] ERROR: Could not get framebuffer information!");
        return false;
    }
    if (fbInfo.bpp != 32) {
        api.kio.println("[gfx_term] ERROR: Only 32bpp framebuffers are currently supported!");
        return false;
    }
    char* buf = (char*)fbInfo.addr;
    memset(buf, 0x00, fbInfo.pitch * fbInfo.height);

    api.kio.println("[gfx_term] Loading background...");
    background = (uint8_t*)api.mm.malloc(fbInfo.height * fbInfo.pitch);
    loadBgImg("/misc/lime2.bmp");
    drawBackground();

    api.kio.println("[gfx_term] Loading font...");
    stream_t font_s = api.vfs.getStream("/misc/font.bmf");
    font = (uint8_t*)api.mm.malloc(font_s.slen);
    stream::read(font_s, (char*)font, font_s.slen);
    stream::close(font_s);

    drawChar('H', 0, 0);
    drawChar('e', 16, 0);
    drawChar('l', 32, 0);
    drawChar('l', 48, 0);
    drawChar('o', 64, 0);

    drawChar('W', 96, 0);
    drawChar('o', 112, 0);
    drawChar('r', 128, 0);
    drawChar('l', 144, 0);
    drawChar('d', 160, 0);
    drawChar('!', 176, 0);

    char* name = genDevName();
    api.kio.print("[gfx_term] Mounting ");
    api.kio.print(name);
    api.kio.println(" ...");

    api.fio.mountStreamProvider(name, FS_FLAG_O_W | FS_FLAG_O_R, _provider);
    api.kio.println("[gfx_term] Done.");
    return true;
}