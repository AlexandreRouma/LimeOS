#include <stdint.h>
#include <interrupts/idt.h>
#include <context/switch_context.h>
//#include <libs/kernel/io.h>
//#include <libs/std/string.h>
//#include <libs/kernel/panic.h>

#define IDT_ENTRY_COUNT 256

struct IDTEntry_t {
   uint16_t offset_0;
   uint16_t selector;
   uint8_t _zero;
   uint8_t type;
   uint16_t offset_1;
};

namespace idt {
    IDTEntry_t _IDT[IDT_ENTRY_COUNT];

    void lidt(void* base, uint16_t size)
    {   // This function works in 32 and 64bit mode
        struct {
            uint16_t length;
            void*    base;
        } __attribute__((packed)) IDTR = { size, base };
    
        asm ( "lidt %0" : : "m"(IDTR) );  // let the compiler choose an addressing mode
    }

    void encodeIdtEntry(uint8_t index, uint32_t offset, uint16_t selector, uint8_t type)
    {
        _IDT[index].offset_0 = (offset >> 0) & 0xFFFF;
        _IDT[index].offset_1 = (offset >> 16) & 0xFFFF;
        _IDT[index].selector = selector * 8; // A GDT entry is 8 bytes long
        _IDT[index].type = type;
        _IDT[index]._zero = 0x00;
    }

    void init() {
        for (int i = 0; i < 256; i++) {
            encodeIdtEntry(i, (uint32_t)&ASM_ISR_0, 0x01, IDT_ATTR_PRESENT | IDT_ATTR_TYPE_TRAP | IDT_ATTR_PRIV_3);
        }

        encodeIdtEntry(0x00, (uint32_t)&_isr0, 0x01, IDT_ATTR_PRESENT | IDT_ATTR_TYPE_INT); // Exceptions
        encodeIdtEntry(0x01, (uint32_t)&_isr1, 0x01, IDT_ATTR_PRESENT | IDT_ATTR_TYPE_INT);
        encodeIdtEntry(0x02, (uint32_t)&_isr2, 0x01, IDT_ATTR_PRESENT | IDT_ATTR_TYPE_INT);
        encodeIdtEntry(0x03, (uint32_t)&_isr3, 0x01, IDT_ATTR_PRESENT | IDT_ATTR_TYPE_INT);
        encodeIdtEntry(0x04, (uint32_t)&_isr4, 0x01, IDT_ATTR_PRESENT | IDT_ATTR_TYPE_INT);
        encodeIdtEntry(0x05, (uint32_t)&_isr5, 0x01, IDT_ATTR_PRESENT | IDT_ATTR_TYPE_INT);
        encodeIdtEntry(0x06, (uint32_t)&_isr6, 0x01, IDT_ATTR_PRESENT | IDT_ATTR_TYPE_INT);
        encodeIdtEntry(0x07, (uint32_t)&_isr7, 0x01, IDT_ATTR_PRESENT | IDT_ATTR_TYPE_INT);
        encodeIdtEntry(0x08, (uint32_t)&_isr8, 0x01, IDT_ATTR_PRESENT | IDT_ATTR_TYPE_INT);
        encodeIdtEntry(0x09, (uint32_t)&_isr9, 0x01, IDT_ATTR_PRESENT | IDT_ATTR_TYPE_INT);
        encodeIdtEntry(0x0A, (uint32_t)&_isr10, 0x01, IDT_ATTR_PRESENT | IDT_ATTR_TYPE_INT);
        encodeIdtEntry(0x0B, (uint32_t)&_isr11, 0x01, IDT_ATTR_PRESENT | IDT_ATTR_TYPE_INT);
        encodeIdtEntry(0x0C, (uint32_t)&_isr12, 0x01, IDT_ATTR_PRESENT | IDT_ATTR_TYPE_INT);
        encodeIdtEntry(0x0D, (uint32_t)&_isr13, 0x01, IDT_ATTR_PRESENT | IDT_ATTR_TYPE_INT);
        encodeIdtEntry(0x0E, (uint32_t)&_isr14, 0x01, IDT_ATTR_PRESENT | IDT_ATTR_TYPE_INT);
        encodeIdtEntry(0x0F, (uint32_t)&_isr15, 0x01, IDT_ATTR_PRESENT | IDT_ATTR_TYPE_INT);
        encodeIdtEntry(0x10, (uint32_t)&_isr16, 0x01, IDT_ATTR_PRESENT | IDT_ATTR_TYPE_INT);
        encodeIdtEntry(0x11, (uint32_t)&_isr17, 0x01, IDT_ATTR_PRESENT | IDT_ATTR_TYPE_INT);
        encodeIdtEntry(0x12, (uint32_t)&_isr18, 0x01, IDT_ATTR_PRESENT | IDT_ATTR_TYPE_INT);
        encodeIdtEntry(0x13, (uint32_t)&_isr19, 0x01, IDT_ATTR_PRESENT | IDT_ATTR_TYPE_INT);
        encodeIdtEntry(0x14, (uint32_t)&_isr20, 0x01, IDT_ATTR_PRESENT | IDT_ATTR_TYPE_INT);
        encodeIdtEntry(0x15, (uint32_t)&_isr21, 0x01, IDT_ATTR_PRESENT | IDT_ATTR_TYPE_INT);
        encodeIdtEntry(0x16, (uint32_t)&_isr22, 0x01, IDT_ATTR_PRESENT | IDT_ATTR_TYPE_INT);
        encodeIdtEntry(0x17, (uint32_t)&_isr23, 0x01, IDT_ATTR_PRESENT | IDT_ATTR_TYPE_INT);
        encodeIdtEntry(0x18, (uint32_t)&_isr24, 0x01, IDT_ATTR_PRESENT | IDT_ATTR_TYPE_INT);
        encodeIdtEntry(0x19, (uint32_t)&_isr25, 0x01, IDT_ATTR_PRESENT | IDT_ATTR_TYPE_INT);
        encodeIdtEntry(0x1A, (uint32_t)&_isr26, 0x01, IDT_ATTR_PRESENT | IDT_ATTR_TYPE_INT);
        encodeIdtEntry(0x1B, (uint32_t)&_isr27, 0x01, IDT_ATTR_PRESENT | IDT_ATTR_TYPE_INT);
        encodeIdtEntry(0x1C, (uint32_t)&_isr28, 0x01, IDT_ATTR_PRESENT | IDT_ATTR_TYPE_INT);
        encodeIdtEntry(0x1D, (uint32_t)&_isr29, 0x01, IDT_ATTR_PRESENT | IDT_ATTR_TYPE_INT);
        encodeIdtEntry(0x1E, (uint32_t)&_isr30, 0x01, IDT_ATTR_PRESENT | IDT_ATTR_TYPE_INT);
        encodeIdtEntry(0x1F, (uint32_t)&_isr31, 0x01, IDT_ATTR_PRESENT | IDT_ATTR_TYPE_INT);

        encodeIdtEntry(0x20, (uint32_t)&ASM_ISR_PIT, 0x01, IDT_ATTR_PRESENT | IDT_ATTR_TYPE_INT);        // PIT
        encodeIdtEntry(0x21, (uint32_t)&ASM_ISR_KBD, 0x01, IDT_ATTR_PRESENT | IDT_ATTR_TYPE_INT);        // Keyboard
        encodeIdtEntry(0x26, (uint32_t)&ASM_ISR_FLOPPY, 0x01, IDT_ATTR_PRESENT | IDT_ATTR_TYPE_INT);     // Floppy
        encodeIdtEntry(0x69, (uint32_t)&ASM_ISR_SYSCALL, 0x01, IDT_ATTR_PRESENT | IDT_ATTR_TYPE_TRAP | IDT_ATTR_PRIV_3);   // Syscall
        encodeIdtEntry(0x42, (uint32_t)&_ASM_SWITCH_TASK_ISR, 0x01, IDT_ATTR_PRESENT | IDT_ATTR_TYPE_TRAP | IDT_ATTR_PRIV_3); 
    }

    void load() {
        lidt(&_IDT, sizeof(_IDT));
    }

    void enableInt() {
        asm("sti");
    }

    void disableInt() {
        asm("cli");
    }
}