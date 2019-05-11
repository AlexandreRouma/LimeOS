#include <vfs/fileio.h>

FSHandler_t _handler;

namespace fio {
    void init() {
        _handler.root = "/fio/";
        _handler.listNodes = listNodes;
        _handler.createNode = createNode;
        _handler.deleteNodes = deleteNode;
        _handler.nodeExists = nodeExists;
        _handler.writable = true;
        _handler.editable = false;
        vfs::registerFSHndlr(_handler);
    }

    vector<FSNode_t> listNodes(char* dir) {
        vector<FSNode_t> f;

        FSNode_t test;
        test.name = "this_works.txt";
        f.push_back(test);

        return f;
    }

    int createNode(char* dir, uint16_t flags) {
        // TODO: Implement
        return 1;
    }

    int deleteNode(char* dir) {
        // TODO: Implement
        return 1;
    }

    bool nodeExists(char* dir) {
        return false;
    }
    
}