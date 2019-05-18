#include <stream.h>
#include <kapi.h>
#include <memory.h>
#include <string.h>

namespace stream {
    stream_t create(uint32_t buffersz, uint64_t slen, uint32_t (*writeHndlr)(stream_t,uint32_t,uint64_t), uint32_t (*readHndlr)(stream_t,uint32_t,uint64_t), void (*closeHndlr)(stream_t), void* tag) {
        stream_t s;
        s.buffersz = buffersz;
        s.slen = slen;
        s.rpos = 0;
        s.wpos = 0;
        s.writeHndlr = writeHndlr;
        s.readHndlr = readHndlr;
        s.closeHndlr = closeHndlr;
        s.tag = tag;
        s.buffer = (char*)kapi::api.mm.malloc(buffersz);
        return s;
    }

    void close(stream_t s) {
        s.closeHndlr(s);
    }

    uint32_t write(stream_t s, char* buf, uint32_t len) {
        int n = len / s.buffersz;
        int last = len % s.buffersz;
        int written = 0;

        for (int i = 0; i < n; i++) {
            memcpy(s.buffer, buf + (s.buffersz * i), s.buffersz);
            int w = s.writeHndlr(s, s.buffersz, s.wpos);
            s.wpos += w;
            written += w;
            if (w == 0) {
                return written;
            }
        }
        int w = 0;
        if (last > 0) {
            memcpy(s.buffer, buf + (s.buffersz * n), last);
            w = s.writeHndlr(s, last, s.wpos);
        }
        s.wpos += w;
        written += w;
        return written;
    }

    uint32_t read(stream_t s, char* buf, uint32_t len) {
        int n = len / s.buffersz;
        int last = len % s.buffersz;
        int read = 0;
        for (int i = 0; i < n; i++) {
            int r = s.readHndlr(s, s.buffersz, s.rpos);
            memcpy(buf + (s.buffersz * i), s.buffer, r);
            s.rpos += r;
            read += r;
            if (r == 0) {
                kapi::api.kio.println("READ ERROR!!!");
                return read;
            }
        }
        int r = 0;
        if (last > 0) {
            r = s.readHndlr(s, last, s.rpos);
            memcpy(buf + (s.buffersz * n), s.buffer, r);
        }
        s.rpos += r;
        read += r;
        return read;
    }
}