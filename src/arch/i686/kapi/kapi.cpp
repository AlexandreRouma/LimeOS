#include <kapi/api/kapi.h>
#include <kernio/kernio.h>
#include <vfs/fileio.h>
#include <misc/liballoc.h>

namespace kapi {
    void init() {
        // kernel IO
        api.kio.print = kio::print;
        api.kio.println = kio::println;

        // FileIO
        api.fio.createDir = fio::createDir;
        api.fio.mountStreamProvider = fio::mountStreamProvider;

        // Memory Management
        api.mm.malloc = malloc;
        api.mm.free = free;
        api.mm.memcpy = memcpy;
    }

    KAPI_t api;
}