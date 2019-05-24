#pragma once
#include <stdint.h>

struct StreamGlobal_t {
    uint64_t wpos;     // Write Position
    uint64_t rpos;     // Read Position
    bool busy;
    bool open;
};

struct stream_t {
    uint32_t buffersz; // Buffer size
    uint64_t slen;     // Stream length
    char* buffer;
    void* tag;

    uint32_t (*writeHndlr)(stream_t,uint32_t,uint64_t);
    uint32_t (*readHndlr)(stream_t,uint32_t,uint64_t);
    void (*closeHndlr)(stream_t);

    StreamGlobal_t* globalData;
};

namespace stream {
    stream_t create(uint32_t buffersz, uint64_t slen, uint32_t (*writeHndlr)(stream_t,uint32_t,uint64_t), uint32_t (*readHndlr)(stream_t,uint32_t,uint64_t), void (*closeHndlr)(stream_t), void* tag);
    uint32_t write(stream_t s, char* buf, uint32_t len);
    uint32_t read(stream_t s, char* buf, uint32_t len);
    void close(stream_t s);
}