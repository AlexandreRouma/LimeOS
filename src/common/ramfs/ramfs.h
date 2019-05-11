#pragma once
#include <stdint.h>
#include <vector.h>
#include <vfs/vfs.h>

struct RAMFSNode_t {
    uint32_t name;
    uint16_t flags;
    uint32_t dataOffset;
    uint32_t dataLen;
    uint32_t subnodes;
    uint32_t subnodesCount;
}  __attribute__((packed));

struct RAMFSHeader_t {
    uint32_t magic;
    uint32_t nameTblOffset;
    uint32_t rootNode;
}  __attribute__((packed));

namespace ramfs {
    bool init(uint32_t addr, char* mount);
    vector<FSNode_t> listNodes(char* dir);
    int createNode(char* path, uint16_t flags);
    int deleteNode(char* path);
    bool nodeExists(char* dir);
}