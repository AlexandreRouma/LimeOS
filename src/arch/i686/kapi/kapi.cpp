#include <kapi/api/kapi.h>
#include <kernio/kernio.h>
#include <vfs/fileio.h>
#include <vfs/vfs.h>
#include <misc/liballoc.h>
#include <paging/paging.h>
#include <kmod/mctl.h>

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
        api.mm.realloc = realloc;
        api.mm.allocPages = paging::allocPages;
        api.mm.setPresent = paging::setPresent;

        // Info
        api.boot_info = info;

        // VFS
        api.vfs.createNode = vfs::createNode;
        api.vfs.deleteNode = vfs::deleteNode;
        api.vfs.listNodes = vfs::listNodes;
        api.vfs.nodeExists = vfs::nodeExists;
        api.vfs.getStream = vfs::getStream;

        // MCTL
        api.mctl.call = mctl::call;
        api.mctl.registerHndlr = mctl::registerHndlr;
    }

    KAPI_t api;
}