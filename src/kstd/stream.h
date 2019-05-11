#pragma once
#include <stdint.h>
#include <misc/liballoc.h>
#include <misc/memory.h>
#include <temp_vga/terminal.h>

class stream {
public:
    stream(uint32_t bufsz, uint64_t len, uint32_t (*_write)(stream*,uint32_t,uint64_t), uint32_t (*_read)(stream*,uint32_t,uint64_t), void (*_close)(stream*), void* _tag);
    stream(const stream& s);
    stream();
    ~stream();

    uint32_t write(char* buf, uint32_t len);
    uint32_t read(char* buf, uint32_t len);
    void close();

    stream & operator=(const stream& s);

    uint32_t buffersz; // Buffer size
    uint64_t slen;     // Stream length
    uint64_t wpos;     // Write Position
    uint64_t rpos;     // Read Position

    char* buffer;
    void* tag;

private:
    uint32_t (*writeHndlr)(stream*,uint32_t,uint64_t);
    uint32_t (*readHndlr)(stream*,uint32_t,uint64_t);
    void (*closeHndlr)(stream*);
};