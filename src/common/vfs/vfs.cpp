#include <vfs/vfs.h>
#include <vector.h>
#include <string.h>
#include <misc/memory.h>
#include <temp_vga/terminal.h>

vector<FSHandler_t> _handlers;

namespace vfs {
    void init() {
        _handlers = vector<FSHandler_t>();
    }

    vector<FSNode_t> listNodes(char* dir) {
        string tempstr = dir;
        if (!tempstr.endWith("/")) {
            tempstr += '/';
        }
        FSHandler_t handler = getHandler(tempstr.c_str());
        vector<FSNode_t> nodes;
        if (!strcmp(handler.root, "")) {
            string subpath = tempstr.substring(strlen(handler.root) - 1);
            if (!subpath.startsWith("/")) {
                subpath = string("/") + subpath;
            }
            nodes = handler.listNodes(subpath.c_str());
        }
        return nodes;
    }

    int createNode(char* dir, uint32_t flags) {
        string tempstr = dir;
        if (!tempstr.endWith("/")) {
            tempstr += '/';
        }
        FSHandler_t handler = getHandler(tempstr.c_str());
        int res;
        if (!strcmp(handler.root, "")) {
            string subpath = tempstr.substring(strlen(handler.root) - 1);
            if (!subpath.startsWith("/")) {
                subpath = string("/") + subpath;
            }
            res = handler.createNode(subpath.c_str(), flags);
        }
        return res;
    }

    int deleteNode(char* dir) {
        string tempstr = dir;
        if (!tempstr.endWith("/")) {
            tempstr += '/';
        }
        FSHandler_t handler = getHandler(tempstr.c_str());
        int res;
        if (!strcmp(handler.root, "")) {
            string subpath = tempstr.substring(strlen(handler.root) - 1);
            if (!subpath.startsWith("/")) {
                subpath = string("/") + subpath;
            }
            res = handler.deleteNodes(subpath.c_str());
        }
        return res;
    }

    bool nodeExists(char* dir) {
        string tempstr = dir;
        if (!tempstr.endWith("/")) {
            tempstr += '/';
        }
        FSHandler_t handler = getHandler(tempstr.c_str());
        bool res;
        if (!strcmp(handler.root, "")) {
            string subpath = tempstr.substring(strlen(handler.root) - 1);
            if (!subpath.startsWith("/")) {
                subpath = string("/") + subpath;
            }
            res = handler.nodeExists(subpath.c_str());
        }
        return res;
    }

    stream getStream(char* dir) {
        string tempstr = dir;
        if (!tempstr.endWith("/")) {
            tempstr += '/';
        }
        FSHandler_t handler = getHandler(tempstr.c_str());
        stream res;
        if (!strcmp(handler.root, "")) {
            string subpath = tempstr.substring(strlen(handler.root) - 1);
            if (!subpath.startsWith("/")) {
                subpath = string("/") + subpath;
            }
            res = handler.getStream(subpath.c_str());
        }
        return res;
    }

    bool registerFSHndlr(FSHandler_t handler) {
        if (handler.root[strlen(handler.root) - 1] != '/') {    
            return false;
        }
        _handlers.push_back(handler);
        return true;
    }

    FSHandler_t getHandler(char* dir) {
        FSHandler_t handler;
        handler.root = "";
        for (int i = 0; i < _handlers.size(); i++) {
            if (strsw(dir, _handlers[i].root) && strlen(_handlers[i].root) > strlen(handler.root)) {
                handler = _handlers[i];
            }
        }
        return handler;
    }
}