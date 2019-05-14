#include <stdint.h>
#include <stdarg.h>
#include <stream.h>

struct FIOAPI_t {
    bool (*createDir)(char*);
    bool (*mountStreamProvider)(char*, uint16_t, stream_t (*)());
};

struct KIOAPI_t {
    void (*print)(char*);
    void (*println)(char*);
};

struct MMAPI_t {
    void* (*malloc)(uint32_t size);
    void (*free)(void*);
    void* (*memcpy)(void*, void*, uint64_t);
};

struct KAPI_t {
    KIOAPI_t kio;
    FIOAPI_t fio;
    MMAPI_t mm;
};

namespace kapi {
    void init();
    extern KAPI_t api;
}