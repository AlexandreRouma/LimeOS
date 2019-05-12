#include <stream.h>

stream::stream(uint32_t bufsz, uint64_t len, uint32_t (*_write)(stream*,uint32_t,uint64_t), uint32_t (*_read)(stream*,uint32_t,uint64_t), void (*_close)(stream*), void* _tag) {
    buffer = (char*)malloc(bufsz);
    buffersz = bufsz;
    slen = len;
    writeHndlr = _write;
    readHndlr = _read;
    closeHndlr = _close;
    tag = _tag;
    wpos = 0;
    rpos = 0;
}

stream::stream(const stream& s) {
    buffer = s.buffer;
    buffersz = s.buffersz;
    slen = s.slen;
    rpos = s.rpos;
    wpos = s.wpos;
    writeHndlr = s.writeHndlr;
    readHndlr = s.readHndlr;
    closeHndlr = s.closeHndlr;
    tag = s.tag;
}

stream::stream() {

}

stream::~stream() {
    
}

void stream::close() {
    closeHndlr(this);
}

uint32_t stream::write(char* buf, uint32_t len) {
    int n = len / buffersz;
    int last = len % buffersz;
    int written = 0;
    for (int i = 0; i < n; i++) {
        memcpy(buffer, buf + (buffersz * i), buffersz);
        int w = writeHndlr(this, buffersz, wpos);
        wpos += w;
        written += w;
        if (w == 0) {
            return written;
        }
    }
    memcpy(buffer, buf + (buffersz * n), buffersz);
    int w = writeHndlr(this, last, wpos);
    wpos += w;
    written += w;
    return written;
}

uint32_t stream::read(char* buf, uint32_t len) {
    int n = len / buffersz;
    int last = len % buffersz;
    int read = 0;
    for (int i = 0; i < n; i++) {
        int r = readHndlr(this, buffersz, rpos);
        memcpy(buf + (buffersz * i), buffer, r);
        rpos += r;
        read += r;
        if (r == 0) {
            return read;
        }
    }
    int r = readHndlr(this, last, rpos);
    memcpy(buf + (buffersz * n), buffer, r);
    rpos += r;
    read += r;
    return read;
}

stream & stream::operator=(const stream& s) {
    buffer = s.buffer;
    buffersz = s.buffersz;
    slen = s.slen;
    rpos = s.rpos;
    wpos = s.wpos;
    writeHndlr = s.writeHndlr;
    readHndlr = s.readHndlr;
    closeHndlr = s.closeHndlr;
    tag = s.tag;
    return *this;
}