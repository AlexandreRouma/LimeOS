#include <kapi/api/kapi.h>
#include <kernio/kernio.h>
#include <vfs/fileio.h>
#include <misc/liballoc.h>

namespace kapi {
    void init(multiboot_info* info) {
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
        api.mm.realloc = realloc;

        // Info
        api.boot_info = info;
    }

    KAPI_t api;
}