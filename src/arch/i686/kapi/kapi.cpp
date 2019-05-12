#include <kapi/kapi.h>
#include <kernio/kernio.h>

namespace kapi {
    void init() {
        api.kio.print = kio::print;
        api.kio.println = kio::println;
    }

    KAPI_t api;
}