#pragma once
#include <stdint.h>
#include <stream.h>

#define FS_FLAG_D       1 << 10
#define FS_FLAG_O_R     1 << 9
#define FS_FLAG_O_W     1 << 8
#define FS_FLAG_O_X     1 << 7
#define FS_FLAG_G_R     1 << 6
#define FS_FLAG_G_W     1 << 5
#define FS_FLAG_G_X     1 << 4
#define FS_FLAG_U_R     1 << 3
#define FS_FLAG_U_W     1 << 2
#define FS_FLAG_U_X     1 << 1
#define FS_FLAG_U_P     1 << 0

struct FSNode_t {
    char* name;
    uint16_t flags;
};

template <class T>
class vector;

struct FSHandler_t {
    char* root;
    vector<FSNode_t> (*listNodes)(char*);
    int (*createNode)(char*, uint16_t flags);
    int (*deleteNodes)(char*);
    bool (*nodeExists)(char*);
    stream_t (*getStream)(char*);
    bool writable;
    bool editable;
};