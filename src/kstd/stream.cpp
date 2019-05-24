#include <stream.h>
#include <scheduler/scheduler.h>

namespace stream {
    stream_t create(uint32_t buffersz, uint64_t slen, uint32_t (*writeHndlr)(stream_t,uint32_t,uint64_t), uint32_t (*readHndlr)(stream_t,uint32_t,uint64_t), void (*closeHndlr)(stream_t), void* tag) {
        stream_t s;
        s.buffersz = buffersz;
        s.slen = slen;
        s.writeHndlr = writeHndlr;
        s.readHndlr = readHndlr;
        s.closeHndlr = closeHndlr;
        s.tag = tag;
        s.buffer = (char*)malloc(buffersz);
        s.globalData = (StreamGlobal_t*)malloc(sizeof(StreamGlobal_t));
        s.globalData->wpos = 0;
        s.globalData->rpos = 0;
        s.globalData->busy = false;
        s.globalData->open = true;
        return s;
    }

    void lock(stream_t s) {
        while (s.globalData->busy) {
            scheduler::yield();
        }
        s.globalData->busy = true;
    }

    void unlock(stream_t s) {
        s.globalData->busy = false;
    }

    void close(stream_t s) {
        if (s.globalData == 0) {
            return;
        }
        lock(s);
        s.globalData->busy = true;
        s.closeHndlr(s);
        free(s.buffer);
        free(s.globalData);
    }

    uint32_t write(stream_t s, char* buf, uint32_t len) {
        if (s.globalData == 0) {
            return 0;
        }
        lock(s);
        int n = len / s.buffersz;
        int last = len % s.buffersz;
        int written = 0;

        for (int i = 0; i < n; i++) {
            memcpy(s.buffer, buf + (s.buffersz * i), s.buffersz);
            int w = s.writeHndlr(s, s.buffersz, s.globalData->wpos);
            s.globalData->wpos += w;
            written += w;
            if (w == 0) {
                return written;
            }
        }
        int w = 0;
        if (last > 0) {
            memcpy(s.buffer, buf + (s.buffersz * n), last);
            w = s.writeHndlr(s, last, s.globalData->wpos);
        }
        s.globalData->wpos += w;
        written += w;
        unlock(s);
        return written;
    }

    uint32_t read(stream_t s, char* buf, uint32_t len) {
        if (s.globalData == 0) {
            return 0;
        }
        lock(s);
        int n = len / s.buffersz;
        int last = len % s.buffersz;
        int read = 0;
        for (int i = 0; i < n; i++) {
            int r = s.readHndlr(s, s.buffersz, s.globalData->rpos);
            memcpy(buf + (s.buffersz * i), s.buffer, r);
            s.globalData->rpos += r;
            read += r;
            if (r == 0) {
                return read;
            }
        }
        int r = 0;
        if (last > 0) {
            r = s.readHndlr(s, last, s.globalData->rpos);
            memcpy(buf + (s.buffersz * n), s.buffer, r);
        }
        s.globalData->rpos += r;
        read += r;
        unlock(s);
        return read;
    }
}