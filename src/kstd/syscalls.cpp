#include <stdint.h>
#include <temp_vga/terminal.h>
#include <string.h>
#include <syscalls.h>

void syscall(uint32_t id, uint32_t ebx, uint32_t ecx, uint32_t edx) {
    asm("int $0x69" :: "a" (id), "b" (ebx), "c" (ecx), "d" (edx));
}

void KERN_EXEC_SYSCALL(uint32_t id, uint32_t ebx, uint32_t ecx, uint32_t edx) {
    if (id == 0x00) {
        char* str = (char*)(ebx + ecx);
        Terminal.print(str);
    }
    if (id == 0x01) {
        //char* c = Keyboard.getKeyChar(Keyboard.readEvent(true));
    }
    else {
        Terminal.print("Syscall!\nID:  0x");
        Terminal.print(dumpHexByte(id >> 24));
        Terminal.print(dumpHexByte(id >> 16));
        Terminal.print(dumpHexByte(id >> 8));
        Terminal.println(dumpHexByte(id >> 0));

        Terminal.print("EBX: 0x");
        Terminal.print(dumpHexByte(ebx >> 24));
        Terminal.print(dumpHexByte(ebx >> 16));
        Terminal.print(dumpHexByte(ebx >> 8));
        Terminal.println(dumpHexByte(ebx >> 0));

        Terminal.print("ECX: 0x");
        Terminal.print(dumpHexByte(ecx >> 24));
        Terminal.print(dumpHexByte(ecx >> 16));
        Terminal.print(dumpHexByte(ecx >> 8));
        Terminal.println(dumpHexByte(ecx >> 0));

        Terminal.print("EDX: 0x");
        Terminal.print(dumpHexByte(edx >> 24));
        Terminal.print(dumpHexByte(edx >> 16));
        Terminal.print(dumpHexByte(edx >> 8));
        Terminal.println(dumpHexByte(edx >> 0));
    }
}