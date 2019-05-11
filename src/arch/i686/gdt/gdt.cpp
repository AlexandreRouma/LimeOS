#include <gdt/gdt.h>
#include <misc/memory.h>

namespace gdt {
    GDTEntry_t _GDT[GDT_ENTRY_COUNT];

    void lgdt(void* base, uint16_t size) {   // This function works in 32 and 64bit mode
        struct {
            uint16_t length;
            void*    base;
        } __attribute__((packed)) GDTR = { size, base };
    
        asm ( "lgdt %0" : : "m"(GDTR) );  // let the compiler choose an addressing mode
    }

    void encodeGdtEntry(uint32_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags) {
        _GDT[index].base_0 = (base >> 0) & 0xFFFF;
        _GDT[index].base_1 = (base >> 16) & 0xFF;
        _GDT[index].base_2 = (base >> 24) & 0xFF;
        _GDT[index].limit_0 = (limit >> 0) & 0xFFFF;
        _GDT[index].limit_1 = (limit >> 16) & 0x0F;
        _GDT[index].access = access;
        _GDT[index].flags = flags & 0x0F;
    }

    void init() {
        asm("cli");

        // Clear GDT
        memset(&_GDT, 0x00, sizeof(_GDT));

        // Code
        encodeGdtEntry(0x01, 0x00000000, 0xFFFFFFFF, GDT_ACCESS_PRESENT | GDT_ACCESS_EXEC | GDT_ACCESS_RW, GDT_FLAG_GR_PAGE | GDT_FLAG_SZ_32B);

        // Data
        encodeGdtEntry(0x02, 0x00000000, 0xFFFFFFFF, GDT_ACCESS_PRESENT | GDT_ACCESS_RW, GDT_FLAG_GR_PAGE | GDT_FLAG_SZ_32B);

        // Stack
        encodeGdtEntry(0x03, 0x00000000, 0x00000000, GDT_ACCESS_PRESENT | GDT_ACCESS_RW | GDT_ACCESS_DIR_D, GDT_FLAG_GR_PAGE | GDT_FLAG_SZ_32B);

        // TSS
        // TODO: Enable once TSS is setup
        //encodeGdtEntry(0x04, (uint32_t)&kernel_tss, 0x68, GDT_ACCESS_PR_TSS | GDT_ACCESS_EXEC | GDT_ACCESS_ACCESSED | GDT_ACCESS_PRIV_3, GDT_FLAG_SZ_32B);
    }

    void load() {
        lgdt(&_GDT, sizeof(_GDT));
        ASM_RELOAD_SEGMENTS();
    }

    void setGDTEntry(uint32_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags) {
        encodeGdtEntry(index, base, limit, access, flags);
    }

    uint32_t findFreeEntry() {
        uint8_t dummy[8];
        memset(&dummy, 0x00, 0x08);
        for (uint32_t i = 1; i < GDT_ENTRY_COUNT; i++) {
            GDTEntry_t e = _GDT[i];
            if (memcmp(&_GDT[i], &dummy, 0x08)) {
                return i;
            }
        }
        return 0;
    }

    uint32_t allocEntry(uint32_t base, uint32_t limit, uint8_t access, uint8_t flags) {
        uint32_t index = findFreeEntry();
        if (index == 0) {
            // TODO: Enable this once kernel panic screen is setup
            //kernel_panic(0x1111, "Ran out of GDT entries :(");
            return 0;
        }
        setGDTEntry(index, base, limit, access, flags);
        return index;
    }

    void freeEntry(uint32_t index) {
        memset(&_GDT[index], 0x00, 0x08);
    }
}