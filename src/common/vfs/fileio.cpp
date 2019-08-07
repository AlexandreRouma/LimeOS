#include <vfs/fileio.h>
#include <map.h>

namespace fio {
    FSHandler_t _handler;
    map<string, FILEIONode_t> _index;

    uint32_t getPathDept(char* str) {
        uint32_t n = 0;
        for (int i = 0; i < strlen(str); i++) {
            if (str[i] == '/') {
                n++;
            }
        }
        return n;
    }

    char* getFilename(char* path) {
        for (int i = strlen(path) - 1; i >= 0; i--) {
            if (path[i] == '/' && i != strlen(path) - 1) {
                return path + i + 1;
            }
        }
        return path;
    }

    void init() {
        _handler.root = "/fio/";
        _handler.listNodes = listNodes;
        _handler.createNode = createNode;
        _handler.deleteNodes = deleteNode;
        _handler.nodeExists = nodeExists;
        _handler.getStream = getStream;
        _handler.writable = true;
        _handler.editable = false;
        vfs::registerFSHndlr(_handler);

        // create basic dirs
        createDir("/dev");
        createDir("/proc");
    }

    vector<FSNode_t> listNodes(char* dir) {
        vector<FSNode_t> v;
        string path = dir;
        if (path.startsWith("/")) {
            path = path.substring(1);
        }
        if (path.endWith("/")) {
            path = path.substring(0, path.length() - 1);
        }
        vector<string>& nodes = _index.getKeys();
        for (int i = 0; i < nodes.size(); i++) {
            string _path = nodes[i];
            if (_path.startsWith(path)) {
                bool valid = false;
                if (getPathDept(_path.c_str()) == 0 && getPathDept(path.c_str()) == 0 && path == "") {
                    valid = true;
                }
                 if (getPathDept(_path.c_str()) == getPathDept(path.c_str()) + 1 && path != "") {
                    valid = true;
                }
                if (valid) {
                    FILEIONode_t tnode = _index[nodes[i]];
                    FSNode_t node;
                    node.name = getFilename(nodes[i].c_str());
                    node.flags = tnode.flags;
                    v.push_back(node);
                }
            }
        }
        return v;
    }

    int createNode(char* dir, uint16_t flags) {
        return 1;
    }

    int deleteNode(char* dir) {
        return 1;
    }

    bool nodeExists(char* dir) {
        if (strcmp(dir, "/")) {
            return true;
        }
        if (strcmp(dir, "")) {
            return false;
        }
        string path = dir;
        if (path.startsWith("/")) {
            path = path.substring(1);
        }
        if (path.endWith("/")) {
            path = path.substring(0, path.length() - 1);
        }
        return _index.exists(path);
    }

    stream_t getStream(char* dir) {
        string path = dir;
        if (path.startsWith("/")) {
            path = path.substring(1);
        }
        if (path.endWith("/")) {
            path = path.substring(0, path.length() - 1);
        }
        return _index[path].provider(_index[path].tag);
    }

    bool createDir(char* dir) {
        if (nodeExists(dir)) {
            return false;
        }
        FILEIONode_t node;
        node.path = dir;
        node.flags = FS_FLAG_D;
        string path = dir;
        if (path.startsWith("/")) {
            path = path.substring(1);
        }
        if (path.endWith("/")) {
            path = path.substring(0, path.length() - 1);
        }
        _index.insert(path, node);
        return true;
    }

    bool mountStreamProvider(char* dir, uint16_t flags, stream_t (*_provider)(void* tag), void* tag) {
        if (nodeExists(dir)) {
            return false;
        }
        FILEIONode_t node;
        node.path = dir;
        node.flags = flags;
        node.provider = _provider;
        node.tag = tag;
        string path = dir;
        if (path.startsWith("/")) {
            path = path.substring(1);
        }
        if (path.endWith("/")) {
            path = path.substring(0, path.length() - 1);
        }
        _index.insert(path, node);
        return true;
    }
}