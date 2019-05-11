#pragma once
#include <stdint.h>
#include <string.h>
#include <stream.h>
#include <stdarg.h>

#define ANSI_COL_BLACK      0x0
#define ANSI_COL_DBLUE      0x1
#define ANSI_COL_DGREEN     0x2
#define ANSI_COL_DCYAN      0x3
#define ANSI_COL_DRED       0x4
#define ANSI_COL_DMAGENTA   0x5
#define ANSI_COL_ORANGE     0x6
#define ANSI_COL_GRAY       0x7
#define ANSI_COL_DGRAY      0x8
#define ANSI_COL_BLUE       0x9
#define ANSI_COL_GREEN      0xA
#define ANSI_COL_CYAN       0xB
#define ANSI_COL_RED        0xC
#define ANSI_COL_MAGENTA    0xD
#define ANSI_COL_YELLOW     0xE
#define ANSI_COL_WHITE      0xF

namespace kio {
    void print(char* str);
    void print(string str);
    void println(char* str);
    void println(string str);
    void printf(char* str, ...);
    void printf(string str, ...);

    extern stream stdout;
}