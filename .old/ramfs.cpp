#include <ramfs/ramfs.h>
#include <vfs/vfs.h>
#include <stream.h>

RAMFSHeader_t* header;

namespace ramfs {
    bool init(uint32_t addr, char* mount) {
        FSHandler_t handler;
        handler.root = mount;
        handler.editable = false;
        handler.writable = false;
        handler.listNodes = listNodes;
        vfs::registerFSHndlr(handler);
        return true;
    }

    vector<FSNode_t> listNodes(char* dir) {
        if (strlen(dir) == 0) {
            return vector<FSNode_t>();
        }

        vector<FSNode_t> nodes;

        // If listing in root, return nodes from header
        if (strcmp(dir, "/")) {
            for (int i = 0; i < header->subnodeCount; i++) {
                FSNode_t subnode;
                subnode.name = header->nodes[i].name;
                subnode.flags = header->nodes[i].flags;
                nodes.push_back(subnode);
            }
            return nodes;
        }

        // Else return from subnode
        RAMFSNode_t node = getNode(dir);
        for (int i = 0; i < header->subnodeCount; i++) {
            FSNode_t subnode;
            subnode.name = node.subnodes[i].name;
            subnode.flags = node.subnodes[i].flags;
            nodes.push_back(subnode);
        }

        return nodes;
    }

    RAMFSNode_t getNode(char* dir) {
        RAMFSNode_t subnode;
        vector<string> parts = string(dir).split('/');
        for (int i = 0; i < header->subnodeCount; i++) {
            if (strcmp(header->nodes[i].name, parts[0].c_str())) {
                subnode = header->nodes[i];
                break;
            }
        }
        for (int i = 1; i < parts.size(); i++) {
            for (int j = 0; j < subnode.subnodeCount; j++) {
                if (strcmp(subnode.subnodes[j].name, parts[i].c_str())) {
                    subnode = header->nodes[j];
                    break;
                }
            }
        }
        return subnode;
    }

    bool nodeExists(char* dir) {
        if (strcmp(dir, "/")) {
            return true;
        }
        bool found = false;
        RAMFSNode_t subnode;
        vector<string> parts = string(dir).split('/');
        for (int i = 0; i < header->subnodeCount; i++) {
            if (strcmp(header->nodes[i].name, parts[0].c_str())) {
                subnode = header->nodes[i];
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }

    }

    FSNode_t* getSubnode(FSNode_t* node) {
        
    }
}