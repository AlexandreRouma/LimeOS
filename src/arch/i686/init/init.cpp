#include <init/init.h>
#include <multiboot/multiboot.h>
#include <gdt/gdt.h>
#include <interrupts/idt.h>
#include <interrupts/pic.h>
#include <paging/paging.h>
#include <pit/pit.h>
#include <temp_vga/terminal.h>
#include <scheduler/scheduler.h>

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

    // Init the scheduler
    scheduler::init();

    // Configure the PIT
    pit::command(PIT_MODE_RATE_GEN | PIT_ACCESS_BOTH | PIT_CHAN_0);
    pit::setFrequency(PIT_PORT_CHAN_0, 2000);

    // Remap PIC
    pic::init();

    // Interrupt Descriptor Table
    idt::init();
    idt::load();
    idt::enableInt();

    // ========== INTERRUPTS ENABLED ==========
}
