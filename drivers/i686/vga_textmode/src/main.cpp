#include <kapi.h>

struct VGAChar_t {
    char c;
    uint8_t fore:4;
    uint8_t back:4;
};

VGAChar_t* fb = (VGAChar_t*)0xB8000;

void printAnsi(char* str) {
    
}

stream_t _stream;

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
    return _stream;
}

extern "C"
bool _start(KAPI_t api) {
    kapi::api = api;
    api.kio.println("[vga_textmode] Mounting tty device...");
    _stream = stream::create(0x1000, 0, _writeHndlr, _readHndlr, _closeHndlr, 0);
    api.fio.mountStreamProvider("/dev/tty0", 0, _provider);
    api.kio.println("[vga_textmode] Done.");
    return true;
}