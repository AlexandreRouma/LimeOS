#pragma once
#include <stdint.h>
#include <string.h>
#include <stream.h>
#include <stdarg.h>

#define ANSI_COL_BLACK      0x0
#define ANSI_COL_DRED       0x1
#define ANSI_COL_DGREEN     0x2
#define ANSI_COL_ORANGE     0x3
#define ANSI_COL_DBLUE      0x4
#define ANSI_COL_DMAGENTA   0x5
#define ANSI_COL_DCYAN      0x6
#define ANSI_COL_GRAY       0x7
#define ANSI_COL_DGRAY      0x8
#define ANSI_COL_RED        0x9
#define ANSI_COL_GREEN      0xA
#define ANSI_COL_YELLOW     0xB
#define ANSI_COL_BLUE       0xC
#define ANSI_COL_MAGENTA    0xD
#define ANSI_COL_CYAN       0xE
#define ANSI_COL_WHITE      0xF

#define LOG_UNDEF          "[      ] "
#define LOG_OK             "\x1B[1;37m[  \x1B[1;32mOK  \x1B[1;37m] "
#define LOG_WARN           "\x1B[1;37m[ \x1B[1;33mWARN \x1B[1;37m] "
#define LOG_FAILED         "\x1B[1;37m[\x1B[1;31mFAILED\x1B[1;37m] "
#define LOG_INFO           "\x1B[1;37m[ \x1B[1;36mINFO \x1B[1;37m] "

namespace kio {
    void print(char* str);
    void print(string str);
    void println(char* str);
    void println(string str);
    void printf(string str, ...);

    void setFore(uint8_t color);
    void setBack(uint8_t color);
    void setCursor(uint32_t x, uint32_t y);
    void setColumn(uint32_t col);
    void clear();

    void ok();
    void failed();
    void warn();
    void info();

    extern stream_t stdout;
}