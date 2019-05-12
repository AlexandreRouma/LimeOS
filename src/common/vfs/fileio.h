#pragma once
#include <stdint.h>
#include <vfs/vfs.h>
#include <vector.h>

struct FILEIONode_t {
    char* path;
    uint16_t flags;
    stream_t s;
};

namespace fio {
    void init();
    vector<FSNode_t> listNodes(char* dir);
    int createNode(char* dir, uint16_t flags);
    int deleteNode(char* dir);
    bool nodeExists(char* dir);
    stream_t getStream(char* dir);

    bool createDir(char* dir);
    bool mountStream(char* dir, uint16_t flags, stream_t s);
}