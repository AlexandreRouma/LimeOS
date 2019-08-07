#pragma once
#include <stdint.h>
#include <vfs/vfs.h>
#include <vector.h>

struct FILEIONode_t {
    char* path;
    uint16_t flags;
    void* tag;
    stream_t (*provider)(void* tag);
};

namespace fio {
    void init();
    vector<FSNode_t> listNodes(char* dir);
    int createNode(char* dir, uint16_t flags);
    int deleteNode(char* dir);
    bool nodeExists(char* dir);
    stream_t getStream(char* dir);

    bool createDir(char* dir);
    bool mountStreamProvider(char* dir, uint16_t flags, stream_t (*_provider)(void* tag), void* tag);
}