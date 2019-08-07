#include <kmod/mctl.h>
#include <map.h>

namespace mctl {
    map<string, MCTLHandler_t> _index;

    void init() {
        _index = map<string, MCTLHandler_t>();
    }

    bool registerHndlr(char* dev, MCTLHandler_t handler) {
        if (_index.exists(dev)) {
            return false;
        }
        _index.insert(dev, handler);
        return true;
    }

    int call(char* dev, uint32_t id, void* in, void* out) {
        string _dev = dev;
        if (!_index.exists(_dev)) {
            return -1;
        }
        return _index[_dev]._handler(_index[_dev].tag, id, in, out);
    }
}

