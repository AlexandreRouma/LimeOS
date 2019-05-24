#include <ramfs/tarfs.h>
#include <vector.h>
#include <string.h>
#include <math.h>
#include <temp_vga/terminal.h>
#include <paging/paging.h>
#include <misc/cpuio.h>
#include <map.h>
#include <kernio/kernio.h>

namespace tarfs {
    map<string, TARFSNode_t> _index;

    uint32_t testpow(uint32_t pow, uint32_t n) {
        uint32_t res = 1;
        for (int i = 0; i < pow; i++) {
            res *= n;
        }
        return res;
    }

    uint64_t tarfs_atoi(char* str) {
        uint32_t len = strlen(str);
        uint32_t n = 0;
        for (int i = 0; i < len; i++) {
            n += (uint32_t)testpow(len - 1 - i, 8) * (str[i] - 0x30);
        }
        return n;
    }

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

    uint32_t offset;
    FSHandler_t _handler;

    void fillIndex() {
        vector<uint32_t> testvec;
        bool done = false;
        uint32_t boffset = 0;
        uint8_t* buffer = (uint8_t*)offset;
        while (!done) {
            if (buffer[boffset] == 0) {
                done = true;
                break;
            }
            char* name = (char*)(offset + boffset);
            

            TARFSNode_t node;
            node.path = name;
            node.flags = tarfs_atoi((char*)(offset + boffset + 100));
            if (buffer[boffset + 156] == '5') {
                node.flags |= FS_FLAG_D;
            }
            string _name = name;
            if (_name.endWith("/")) {
                _name = _name.substring(0, _name.length() - 1);
            }
            if (_name.startsWith("/")) {
                _name = _name.substring(1);
            }
            uint32_t length = tarfs_atoi((char*)(offset + boffset + 124));
            node.size = length;
            node.data = (char*)(offset + boffset + 512);

            _index.insert(_name, node); // copy name

            boffset += (length / 512) * 512;
            if (length % 512 > 0) {
                boffset += 512;
            }
            boffset += 512;
            
        }
    }

    bool init(uint32_t addr, char* mount) {
        offset = addr;
        _index = map<string, TARFSNode_t>();
        // TODO: Mount handler + index
        _handler.listNodes = listNodes;
        _handler.createNode = createNode;
        _handler.deleteNodes = deleteNode;
        _handler.nodeExists = nodeExists;
        _handler.getStream = getStream;
        _handler.root = mount;

        fillIndex();

        return vfs::registerFSHndlr(_handler);
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
                    TARFSNode_t tnode = _index[nodes[i]];
                    FSNode_t node;
                    node.name = getFilename(nodes[i].c_str());
                    node.flags = tnode.flags;
                    v.push_back(node);
                }
            }
        }
        return v;
    }

    int createNode(char* path, uint16_t flags) {
        return 1;
    }

    int deleteNode(char* path) {
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

    TARFSNode_t getTARFSNode(char* dir) {
        if (strcmp(dir, "/")) {
            TARFSNode_t node;
            node.path = 0;
            node.flags = 0;
            node.data = 0;
            node.size = 0;
            node.path = "/";
            node.flags = FS_FLAG_D | FS_FLAG_U_R | FS_FLAG_U_X;
            return node;
        }
        string path = dir;
        if (path.startsWith("/")) {
            path = path.substring(1);
        }
        if (path.endWith("/")) {
            path = path.substring(0, path.length() - 1);
        }
        return _index[path];
    }

    uint32_t _writeHndlr(stream_t s, uint32_t len, uint64_t pos) {
        return 0;
    }

    uint32_t _readHndlr(stream_t s, uint32_t len, uint64_t pos) {
        if (s.buffer != 0) {
            memcpy(s.buffer, s.tag + pos, len);
            return len;
        }
        return 0;
    }

    void _closeHndlr(stream_t s) {
        
    }

    stream_t getStream(char* dir) {
        TARFSNode_t node = getTARFSNode(dir);
        if (node.data == 0) {
            return stream_t();
        }
        return stream::create(0x1000, node.size, _writeHndlr, _readHndlr, _closeHndlr, node.data);
    }
}