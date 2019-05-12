#include <stdint.h>
#include <stdarg.h>
#include <stream.h>

struct FIOAPI_t {
    bool (*createDir)(char*);
    bool (*mountStream)(char*, stream_t);
};

struct KIOAPI_t {
    void (*print)(char*);
    void (*println)(char*);
};

struct KAPI_t {
    KIOAPI_t kio;
    FIOAPI_t fio;
};

namespace kapi {
    void init();
    extern KAPI_t api;
}