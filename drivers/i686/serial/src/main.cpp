#include <kapi.h>
#include <memory.h>
#include <string.h>
#include <cpuio.h>

uint32_t _writeHndlr(stream_t s, uint32_t len, uint64_t pos) {
    for (int i = 0; i < len; i++) {
        outb(0x3F8, s.buffer[i]);
    }
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

extern "C"
bool _start(KAPI_t api) {
    kapi::api = api;
    api.kio.println("[serial] Initializing serial...");

    uint32_t addr = 0x3F8;
    uint32_t baud = 115200;

    uint8_t lcr = inb(addr + 0x03);
    lcr |= 0x80;
    outb(addr + 0x03, lcr);
    uint16_t divisor = 115200 / baud;
    outb(addr, divisor & 0x00FF);
    outb(addr + 0x01, (divisor >> 8) & 0x00FF);
    lcr = inb(addr + 0x03);
    lcr &= 0x7F;
    outb(addr + 0x03, lcr);

    api.kio.println("[serial] Mounting /dev/ttySER0...");
    api.fio.mountStreamProvider("/dev/ttySER0", 0, _provider);
    api.kio.println("[serial] Done.");
    return true;
}