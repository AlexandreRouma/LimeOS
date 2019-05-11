#include <ramfs/ramfs.h>
#include <string.h>
#include <temp_vga/terminal.h>
#include <vfs/vfs.h>

namespace ramfs {
    uint32_t offset;
    RAMFSHeader_t* header;
    RAMFSNode_t* rootNode;
    FSHandler_t _handler;

    bool init(uint32_t addr, char* mount) {
        offset = addr;
        header = (RAMFSHeader_t*)addr;
        // TODO: Mount handler
        _handler.listNodes = listNodes;
        _handler.createNode = createNode;
        _handler.deleteNodes = deleteNode;
        _handler.nodeExists = nodeExists;
        return vfs::registerFSHndlr(_handler);
    }

    
    RAMFSNode_t* getSubnode(RAMFSNode_t* node, char* name) {
        if (node->subnodes == 0 || node->subnodesCount == 0) {
            return NULL;
        }
        RAMFSNode_t* subNodes = (RAMFSNode_t*)(node->subnodes + offset);
        for (int i = 0; i < node->subnodesCount; i++) {
            char* _name = (char*)(subNodes[i].name + header->nameTblOffset + offset);
            if (strcmp(_name, name)) {
                return &subNodes[i];
            }
        }
        return NULL;
    }

    vector<FSNode_t> listNodes(char* dir) {
        // Condition path
        string path = dir;
        if (path.startsWith("/")) {
            path = path.substring(1, path.length() - 1);
        }
        if (path.endWith("/")) {
            //path = path.substring(0, path.length() - 1);
        }
        
        // Split path
        vector<string> parts = path.split('/');
        for (int i = 0; i < parts.size(); i++) {
            Terminal.println(parts[i]);
        }

        RAMFSNode_t* parent = rootNode;
        vector<FSNode_t> nodes;

        for (int i = 0; i < parts.size(); i++) {
            parent = getSubnode(parent, parts[i].c_str());
            if (parent == NULL || parent->flags & FS_FLAG_D == 0) {
                return nodes; // Will be empty
            }
        }

        RAMFSNode_t* subnodes = (RAMFSNode_t*)(parent->subnodes + offset);

        for (int i = 0; i < parent->subnodesCount; i++) {
            FSNode_t node;
            node.flags = subnodes[i].flags;
            node.name = (char*)(subnodes[i].name + header->nameTblOffset + offset);
            nodes.push_back(node);
        }

        return nodes;
    }

    int createNode(char* path, uint16_t flags) {
        return 1;
    }

    int deleteNode(char* path) {
        return 1;
    }

    bool nodeExists(char* dir) {
        // Condition path
        string path = dir;
        if (path.startsWith("/")) {
            path = path.substring(1, path.length() - 1);
        }
        if (path.endWith("/")) {
            //path = path.substring(0, path.length() - 1);
        }
        
        // Split path
        vector<string> parts = path.split('/');
        for (int i = 0; i < parts.size(); i++) {
            Terminal.println(parts[i]);
        }

        RAMFSNode_t* parent = rootNode;
        vector<FSNode_t> nodes;

        for (int i = 0; i < parts.size(); i++) {
            parent = getSubnode(parent, parts[i].c_str());
            if (parent == NULL || ((parent->flags & FS_FLAG_D == 0) && i < parts.size() - 1)) {
                return false;
            }
        }

        return true;
    }
}