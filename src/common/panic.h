#pragma once
#include <temp_vga/terminal.h>

inline void kpanic(char* msg, uint32_t err) {
    Terminal.print("PANIC: ");
    Terminal.println(msg);
    Terminal.print("CODE: 0x");
    Terminal.println(itoa(err, 16));
    while (1);
}