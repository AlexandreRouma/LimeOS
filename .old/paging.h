#pragma once
#include <stdint.h>

#define PD_PRESENT  1 << 0
#define PD_RW       1 << 1
#define PD_ALL_PRIV 1 << 2
#define PD_WRITETHR 1 << 3
#define PD_CACHE_D  1 << 4
#define PD_ACCESSED 1 << 5
#define PD_4M_PAGE  1 << 7

#define PT_PRESENT  1 << 0
#define PT_RW       1 << 1
#define PT_ALL_PRIV 1 << 2
#define PT_WRITETHR 1 << 3
#define PT_CACHE_D  1 << 4
#define PT_ACCESSED 1 << 5
#define PT_DIRTY    1 << 6
#define PT_GLOBAL   1 << 8

namespace paging {
    void enable(uint32_t maxmem);
    void mapPage(uint32_t phy, uint32_t virt, uint16_t flags);
    uint32_t getPhysicalAddr(uint32_t virt);
    uint16_t getFlags(uint32_t virt);
    void setFlags(uint32_t virt, uint16_t flags);
    uint16_t getDirectoryFlags(uint32_t virt);
    void setDirectoryFlags(uint32_t virt, uint16_t flags);
    void setPresent(uint32_t virt, uint32_t count);
    void setAbsent(uint32_t virt, uint32_t count);
    uint32_t findPages(uint32_t count);
    uint32_t allocPages(uint32_t count);
    uint32_t getUsedPages();
    void invlpg(uint32_t addr);
};

extern "C" 
{
    extern void ASM_LOAD_PAGE_DIRECTORY(uint32_t*);
    extern void ASM_ENABLE_PAGING(void);
}