#pragma once
#include <stdint.h>
#include <vector.h>
#include <vfs/vfs.h>
#include <stream.h>

struct TARFSNode_t {
    char* data;
    char* path;
    uint32_t size;
    uint16_t flags;
};

namespace tarfs {
    bool init(uint32_t addr, char* mount);
    vector<FSNode_t> listNodes(char* dir);
    int createNode(char* path, uint16_t flags);
    int deleteNode(char* path);
    TARFSNode_t getTARFSNode(char* dir);
    bool nodeExists(char* dir);
    //FSNode_t getNode(char* path);
    stream_t getStream(char* path);
}