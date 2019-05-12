#pragma once
#include <stdint.h>
#include <misc/liballoc.h>
#include <misc/memory.h>
#include <temp_vga/terminal.h>

struct stream_t {
    uint32_t buffersz; // Buffer size
    uint64_t slen;     // Stream length
    uint64_t wpos;     // Write Position
    uint64_t rpos;     // Read Position
    char* buffer;
    void* tag;

    uint32_t (*writeHndlr)(stream_t,uint32_t,uint64_t);
    uint32_t (*readHndlr)(stream_t,uint32_t,uint64_t);
    void (*closeHndlr)(stream_t);
};

namespace stream {
    stream_t create(uint32_t buffersz, uint64_t slen, uint32_t (*writeHndlr)(stream_t,uint32_t,uint64_t), uint32_t (*readHndlr)(stream_t,uint32_t,uint64_t), void (*closeHndlr)(stream_t), void* tag);
    uint32_t write(stream_t s, char* buf, uint32_t len);
    uint32_t read(stream_t s, char* buf, uint32_t len);
    void close(stream_t s);
}