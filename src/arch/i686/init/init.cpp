#include <init/init.h>
#include <multiboot/multiboot.h>
#include <gdt/gdt.h>
#include <interrupts/idt.h>
#include <interrupts/pic.h>
#include <paging/paging.h>
#include <temp_vga/terminal.h>

void _init(multiboot_info* multiboot_info) {
    // Temporary VGA for debug
    Terminal.init(80, 25);
    Terminal.clear();

    // Global Descriptor Table
    gdt::init();
    gdt::load();

    // Paging
    paging::init();
    paging::setPresent(0, ((uint32_t)ASM_KERNEL_END / 4096) + 1); // Allocate for the kernel
    paging::enable();

    // ========== MEMORY MANAGEMENT ENABLED ==========

    // Remap PIC
    pic::init();

    // Interrupt Descriptor Table
    idt::init();
    idt::load();
    idt::enableInt();

    // ========== INTERRUPTS ENABLED ==========
}
