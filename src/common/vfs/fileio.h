#include <stdint.h>
#include <vfs/vfs.h>
#include <vector.h>

namespace fio {
    void init();
    vector<FSNode_t> listNodes(char* dir);
    int createNode(char* dir, uint16_t flags);
    int deleteNode(char* dir);
    bool nodeExists(char* dir);
}