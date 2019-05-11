#pragma once
#include <stdint.h>

#define GDT_ACCESS_PRESENT  0x90
#define GDT_ACCESS_PR_TSS   0x80
#define GDT_ACCESS_PRIV_3   0x60
#define GDT_ACCESS_PRIV_2   0x40
#define GDT_ACCESS_PRIV_1   0x20
#define GDT_ACCESS_EXEC     0x08
#define GDT_ACCESS_DIR_D    0x04
#define GDT_ACCESS_RW       0x02
#define GDT_ACCESS_ACCESSED 0x01

#define GDT_FLAG_GR_PAGE    0x08
#define GDT_FLAG_SZ_32B     0x04

#define GDT_ENTRY_COUNT 256

struct GDTEntry_t {
    uint16_t limit_0;
    uint16_t base_0;
    uint8_t base_1;
    uint8_t access;
    uint8_t limit_1:4;
    uint8_t flags:4;
    uint8_t base_2;
} __attribute__((packed));

namespace gdt {
    void init();
    void load();
    void setGDTEntry(uint32_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags);
    uint32_t findFreeEntry();
    uint32_t allocEntry(uint32_t base, uint32_t limit, uint8_t access, uint8_t flags);
    void freeEntry(uint32_t index);
}

extern "C" 
{
    extern void ASM_RELOAD_SEGMENTS(void);
}