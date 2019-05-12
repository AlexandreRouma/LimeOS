#include <kapi/kapi.h>

extern "C"
bool _start(KAPI_t api) {
    api.print("This was printed via the module api!!!");
    return true;
}