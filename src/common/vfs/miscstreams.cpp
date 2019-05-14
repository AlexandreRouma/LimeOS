#include <vfs/miscstreams.h>
#include <vfs/fileio.h>
#include <misc/random.h>

namespace miscstrms {
    stream_t _nullStream;
    stream_t _randomStream;

    uint32_t _null_writeHndlr(stream_t s, uint32_t len, uint64_t pos) {
        return len;
    }

    uint32_t _null_readHndlr(stream_t s, uint32_t len, uint64_t pos) {
        memset(s.buffer, 0, len);
        return len;
    }

    uint32_t _random_writeHndlr(stream_t s, uint32_t len, uint64_t pos) {
        return 0;
    }

    uint32_t _random_readHndlr(stream_t s, uint32_t len, uint64_t pos) {
        for (int i = 0; i < len; i++) {
            s.buffer[i] = rand::get() % 256;
        }
        return len;
    }

    void _closeHndlr(stream_t s) {
        
    }

    stream_t _nullProvider() {
        return _nullStream;
    }

    stream_t _randomProvider() {
        return _randomStream;
    }

    void init() {
        _nullStream = stream::create(0x1000, 0, _null_writeHndlr, _null_readHndlr, _closeHndlr, NULL);
        _randomStream = stream::create(0x1000, 0, _random_writeHndlr, _random_readHndlr, _closeHndlr, NULL);
        fio::mountStreamProvider("/dev/null",  FS_FLAG_O_R | FS_FLAG_O_W, _nullProvider);
        fio::mountStreamProvider("/dev/random",  FS_FLAG_O_R | FS_FLAG_O_W, _randomProvider);
    }
}