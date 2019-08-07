#include <kscript/kscript.h>
#include <vfs/vfs.h>
#include <panic.h>
#include <kernio/kernio.h>
#include <map.h>
#include <kmod/modules.h>

namespace ksc {
    map<string, void (*)(string, vector<string>)> commands;

    // <=== COMMANDS ===>
    void _cmd_print(string text, vector<string> args) {
        kio::println(text);
    }

    void _cmd_stdout(string text, vector<string> args) {
        if (!vfs::nodeExists(text.c_str())) {
            kpanic("Tried to switch stdout to non-existant file", 0);
            return;
        }
        kio::stdout = vfs::getStream(text.c_str());
    }

    void _cmd_loadmod(string text, vector<string> args) {
        if (!vfs::nodeExists(text.c_str())) {
            kpanic("Tried to load inexistant module", 0);
            return;
        }
        kmod::load(text.c_str());
    }
    // <=== COMMANDS ===>

    void init() {
        commands = map<string, void (*)(string, vector<string>)>();
        commands.insert("print", _cmd_print);
        commands.insert("stdout", _cmd_stdout);
        commands.insert("loadmod", _cmd_loadmod);
    }

    void run(char* path) {
        // TODO: Make error messages more descriptive
        if (!vfs::nodeExists(path)) {
            kpanic("The kscript does not exist", 0);
            return;
        }
        // TODO: Check if node is a file
        stream_t s = vfs::getStream(path);
        char* buf = (char*)malloc(s.slen + 1);
        stream::read(s, buf, s.slen);
        buf[s.slen] = 0;
        stream::close(s);

        string text = buf;
        vector<string> lines = text.split('\n');
        for (int i = 0; i < lines.size(); i++) {
            if (lines[i] == "" || lines[i].startsWith("#")) {
                continue;
            }
            vector<string> args = lines[i].split(' ');
            string text = "";
            if (args.size() > 1) {
                text = lines[i].substring(args[0].length() + 1);
            }
            if (!commands.exists(args[0])) {
                kpanic("Invalid command in kscript", 0);
            }
            commands[args[0]](text, args);
        }
    }
}