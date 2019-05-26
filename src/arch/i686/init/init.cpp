#include <init/init.h>
#include <multiboot/multiboot.h>
#include <gdt/gdt.h>
#include <interrupts/idt.h>
#include <interrupts/pic.h>
#include <paging/paging.h>
#include <pit/pit.h>
#include <temp_vga/terminal.h>
#include <scheduler/scheduler.h>
#include <elf/elf.h>

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

    // Protect modules
    multiboot_module_t* mods = (multiboot_module_t*)multiboot_info->mods_addr;
    for (int i = 0; i < multiboot_info->mods_count; i++) {
        paging::setPresent(mods[i].mod_start, ((mods[i].mod_end - mods[i].mod_start) / 4096) + 1);
    }

    // Protect kernel symbols
    paging::setPresent(multiboot_info->u.elf_sec.addr, paging::sizeToPages(multiboot_info->u.elf_sec.size * multiboot_info->u.elf_sec.num));
    ELFSectionHeader_t* kern_sections = (ELFSectionHeader_t*)multiboot_info->u.elf_sec.addr;
    for (int i = 0; i < multiboot_info->u.elf_sec.num; i++) {
        paging::setPresent(kern_sections[i].addr, paging::sizeToPages(kern_sections[i].size));
    }

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
