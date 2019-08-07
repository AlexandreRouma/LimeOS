#include <string.h>
#include <stream.h>
#include <vfs/fileio.h>
#include <paging/paging.h>
#include <kernio/kernio.h>
#include <misc/cpuio.h>

uint32_t TERM_WIDTH = 80;
uint32_t  TERM_HEIGHT = 25;

struct VGAChar_t {
    char c;
    uint8_t fore:4;
    uint8_t back:4;
};

const uint8_t ANSI_COLOR_PALLET[16] = {
    0, 4, 2, 6, 1, 5, 3, 7, 8, 12, 10, 14, 9, 13, 11, 15
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

void updateCursor(int x, int y)
{
	uint16_t pos = (y * TERM_WIDTH) + x;
	outb(0x3D4, 0x0F);
	outb(0x3D5, pos & 0xFF);
	outb(0x3D4, 0x0E);
	outb(0x3D5, (pos >> 8) & 0xFF);
}

int runEscCode(char* code) {
    uint32_t codeLen = strlen(code);
    uint32_t last = 0;
    if (code[0] == '[') {
        uint32_t num = 0;
        vector<int> values;
        char suffix = '\0';
        for (int i = 1; i < codeLen; i++) {
            if (code[i] == ';') {
                values.push_back(num);
                num = 0;
                continue;
            }
            if (code[i] >= '0' && code[i] <= '9') {
                num *= 10;
                num += code[i] - '0';
                continue;
            }
            values.push_back(num);
            suffix = code[i];
            last = i;
            break;
        }

        if (suffix == 'm') {
            uint8_t intensity = 0;
            for (int i = 0; i < values.size(); i++) {
                if (values[i] == 1) {
                    intensity = 1 << 3;
                }
                if (values[i] >= 30 && values[i] <= 37) {
                    fgColor = ANSI_COLOR_PALLET[(values[i] - 30) | intensity];
                }
                if (values[i] >= 40 && values[i] <= 47) {
                    bgColor =ANSI_COLOR_PALLET[(values[i] - 40) | intensity];
                }
            }
        }
        else if (suffix == 'H') {
            if (values.size() != 2) {
                return last + 1;
            }
            int posx = (values[1] + (values[1] == 0)) - 1;
            int posy = (values[0] + (values[0] == 0)) - 1;
            if (posx >= TERM_WIDTH || posy >= TERM_HEIGHT) {
                return last + 1;
            }
            cursorX = posx;
            cursorY = posy;
        }
        else if (suffix == 'G') {
            if (values.size() != 1) {
                return last + 1;
            }
            int posx = (values[0] + (values[0] == 0)) - 1;
            if (posx >= TERM_WIDTH) {
                return last + 1;
            }
            cursorX = posx;
        }
    }
    return last + 1;
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
    updateCursor(cursorX, cursorY);
}

uint32_t _writeHndlr(stream_t s, uint32_t len, uint64_t pos) {
    char* str = (char*)malloc(len + 1);
    memcpy(str, s.buffer, len);
    str[len] = 0;
    print(str);
    free(str);
    return len;
}

uint32_t _readHndlr(stream_t s, uint32_t len, uint64_t pos) {
    return 0;
}

void _closeHndlr(stream_t s) {
    
}

stream_t _provider(void* tag) {
    return stream::create(0x1000, 0, _writeHndlr, _readHndlr, _closeHndlr, 0);;
}

extern "C"
bool _start() {
    kio::println("[vga_textmode] HELLO...");
    cursorX = 0;
    cursorY = 0;
    fgColor = 0xF;
    bgColor = 0x0;
    fb = (VGAChar_t*)0xB8000;//api.boot_info->framebuffer_addr;
    TERM_WIDTH = 80;//api.boot_info->framebuffer_width;
    TERM_HEIGHT = 25;//api.boot_info->framebuffer_height;
    kio::println("[vga_textmode] HELLO2...");
    paging::setPresent(0xB8000, 2);
    kio::println("[vga_textmode] Mounting tty device...");
    fio::mountStreamProvider("/dev/tty0", 0, _provider, NULL);
    kio::println("[vga_textmode] Done.");
    for (int y = 0; y < TERM_HEIGHT; y++) {
        for (int x = 0; x < TERM_WIDTH; x++) {
            fb[(y * TERM_WIDTH) + x].c = ' ';
        }
    }
    return true;
}