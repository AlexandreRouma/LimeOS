#pragma once
#include <temp_vga/terminal.h>

inline void kpanic(char* msg, uint32_t err) {
    Terminal.print("PANIC: ");
    Terminal.println(msg);
    Terminal.print("CODE: 0x");
    //Terminal.println(itoa(err, 16));
    Terminal.print(dumpHexByte((err >> 24) & 0xFF));
    Terminal.print(dumpHexByte((err >> 16) & 0xFF));
    Terminal.print(dumpHexByte((err >> 8) & 0xFF));
    Terminal.println(dumpHexByte((err >> 0) & 0xFF));
    while (1);
}