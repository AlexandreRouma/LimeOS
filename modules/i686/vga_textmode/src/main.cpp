#include <kapi.h>
#include <string.h>

uint32_t TERM_WIDTH = 80;
uint32_t  TERM_HEIGHT = 25;

struct VGAChar_t {
    char c;
    uint8_t fore:4;
    uint8_t back:4;
};

VGAChar_t* fb = (VGAChar_t*)0xB8000;
uint32_t cursorX = 0;
uint32_t cursorY = 0;

uint8_t fgColor = 0xF;
uint8_t bgColor = 0x0;

void scrollUp(int n = 1) {
    for (int i = 0; i < n; i++) {
        memcpy(&fb[0], &fb[TERM_WIDTH], TERM_WIDTH* (TERM_HEIGHT - 1) * 2);
        for (int i = 0; i < TERM_WIDTH; i++) {
            fb[(TERM_WIDTH * (TERM_HEIGHT - 1)) + i].c = ' ';
        }
    }
}

void newLine() {
    cursorX = 0;
    if (cursorY >= TERM_HEIGHT - 1) {
        scrollUp();
        return;
    }
    cursorY++;
}

int runEscCode(char* code) {
    if (code[0] == '[') {
        uint8_t intensity = 0;
        for (int i = 1; i < strlen(code); i++) {
            if (code[i] == '1') {
                intensity = 1;
                i++;
            }
            else if (code[i] == '3' && code[i + 2] == 'm') {
                i++;
                if (code[i] > '9' || code[i] < '0') {
                    return i + 1;
                }
                fgColor = (code[i] - '0') | (intensity << 3);
                i++;
            }
            else if (code[i] == '4' && code[i + 2] == 'm') {
                i++;
                if (code[i] > '9' || code[i] < '0') {
                    return i + 1;
                }
                bgColor = (code[i] - '0') | (intensity << 3);
                i++;
            }
            else {
                return i + 1;
            }
            if (code[i] != ';') {
                return i + 1;
            }
        }
    }
    return 1;
}

void print(char* str) {
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        if (str[i] == '\n') {
            newLine();
            continue;
        }
        else if (str[i] == '\x1B') {
            i += runEscCode(&str[i + 1]);
        }
        else {
            fb[(cursorY * TERM_WIDTH) + cursorX].c = str[i];
            fb[(cursorY * TERM_WIDTH) + cursorX].fore = fgColor;
            fb[(cursorY * TERM_WIDTH) + cursorX].back = bgColor;
            cursorX++;
            if (cursorX >= TERM_WIDTH) {
                newLine();
            }
            continue;
        }  
    }
}

uint32_t _writeHndlr(stream_t s, uint32_t len, uint64_t pos) {
    char* str = (char*)kapi::api.mm.malloc(len + 1);
    memcpy(str, s.buffer, len);
    str[len] = 0;
    print(str);
    kapi::api.mm.free(str);
    return len;
}

uint32_t _readHndlr(stream_t s, uint32_t len, uint64_t pos) {
    return 0;
}

void _closeHndlr(stream_t s) {
    
}

stream_t _provider() {
    return stream::create(0x1000, 0, _writeHndlr, _readHndlr, _closeHndlr, 0);;
}

extern "C"
bool _start(KAPI_t api) {
    kapi::api = api;
    cursorX = 0;
    cursorY = 0;
    fgColor = 0xF;
    bgColor = 0x0;
    fb = (VGAChar_t*)0xB8000;//api.boot_info->framebuffer_addr;
    TERM_WIDTH = 80;//api.boot_info->framebuffer_width;
    TERM_HEIGHT = 25;//api.boot_info->framebuffer_height;
    api.mm.setPresent(0xB8000, 2);
    api.kio.println("[vga_textmode] Mounting tty device...");
    api.fio.mountStreamProvider("/dev/tty0", 0, _provider);
    api.kio.println("[vga_textmode] Done.");
    for (int y = 0; y < TERM_HEIGHT; y++) {
        for (int x = 0; x < TERM_WIDTH; x++) {
            fb[(y * TERM_WIDTH) + x].c = ' ';
        }
    }
    return true;
}