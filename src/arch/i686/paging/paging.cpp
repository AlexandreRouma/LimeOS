#include <stdint.h>
#include <paging/paging.h>
#include <temp_vga/terminal.h>

namespace paging {
    uint32_t page_directory[1024] __attribute__((aligned(4096)));
    uint32_t page_tables[1024][1024] __attribute__((aligned(4096)));

    void init() {
        // Init page directory
        for (uint32_t i = 0; i < 1024; i++) {
            page_directory[i] = PD_RW;
        }

        // Init page tables
        for (uint32_t i = 0; i < 1024; i++) {
            for (uint32_t j = 0; j < 1024; j++) {
                page_tables[i][j] = ((j * 0x1000) + (i * 0x400000)) | PT_RW;
            }
        }

        // Fill page directory with page tables
        for (uint32_t i = 0; i < 1024; i++) {
            page_directory[i] = ((uint32_t)&page_tables[i][0]) | PD_RW | PD_PRESENT;
        }
    }

    void enable() {
        ASM_LOAD_PAGE_DIRECTORY(page_directory);
        ASM_ENABLE_PAGING();
    }

    void mapPage(uint32_t phy, uint32_t virt, uint16_t flags) {
        uint32_t pdi = virt >> 22;
        uint32_t pti = virt >> 12 & 0x03FF;
        page_tables[pdi][pti] = phy | flags;
        invlpg(virt);
    }

    uint32_t getPhysicalAddr(uint32_t virt) {
        uint32_t pdi = virt >> 22;
        uint32_t pti = virt >> 12 & 0x03FF;
        return page_tables[pdi][pti] & 0xFFFFF000;
    }

    uint16_t getFlags(uint32_t virt) {
        uint32_t pdi = virt >> 22;
        uint32_t pti = virt >> 12 & 0x03FF;
        return page_tables[pdi][pti] & 0xFFF;
    }

    void setFlags(uint32_t virt, uint32_t count, uint16_t flags) {
        uint32_t page_n = virt / 4096;
        for (uint32_t i = page_n; i < page_n + count; i++) {
            page_tables[i / 1024][i % 1024] &= 0xFFFFF000; // Clear flags
            page_tables[i / 1024][i % 1024] |= flags;
            invlpg(i * 4096);
        }
    }

    uint16_t getDirectoryFlags(uint32_t virt) {
        return page_directory[virt >> 22] & 0xFFF;
    }

    void setDirectoryFlags(uint32_t virt, uint32_t count, uint16_t flags) {
        uint32_t pdi = virt >> 22;
        for (uint32_t i = pdi; i < pdi + count; i++) {
            page_directory[i] &= 0xFFFFF000;
            page_directory[i] |= flags;
        }
    }

    void setPresent(uint32_t virt, uint32_t count) {
        uint32_t page_n = virt / 4096;
        for (uint32_t i = page_n; i < page_n + count; i++) {
            page_tables[i / 1024][i % 1024] |= PT_PRESENT;
            invlpg(i * 4096);
        }
    }

    void setAbsent(uint32_t virt, uint32_t count) {
        uint32_t page_n = virt / 4096;
        for (uint32_t i = page_n; i < page_n + count; i++) {
            page_tables[i / 1024][i % 1024] &= 0xFFFFFFFE; // Clears first bit. Can't use PT_PRESENT because its not a uint32_t
            invlpg(i * 4096);
        }
    }

    uint32_t findPages(uint32_t count) {
        uint32_t continous = 0;
        for (uint32_t i = 0; i < 1024; i++) {
            for (uint32_t j = 0; j < 1024; j++) {
                if ((page_tables[i][j] & PT_PRESENT) == 0) {
                    continous++;
                }
                else {
                    continous = 0;
                }
                if (continous == count) {
                    return (i * 0x400000) + (j * 0x1000);
                }
            }
        }
        // TODO: Add a kernel panic
        //kernel_panic(0xF0CC, "Out of memory :/");
        return 0;
    }

    uint32_t allocPages(uint32_t count) {
        uint32_t ptr = findPages(count);
        setPresent(ptr, count);
        return ptr;
    }

    uint32_t getUsedPages() {
        uint32_t n = 0;
        for (uint32_t i = 0; i < 1024; i++) {
            for (uint32_t j = 0; j < 1024; j++) {
                uint8_t flags = page_tables[i][j] & 0x01;
                if (flags == 1) {
                    n++;
                }
            }
        }
        return n;
    }

    uint32_t sizeToPages(uint32_t size) {
        return (size / 4096) + 1;
    }

    void invlpg(uint32_t addr) {
        asm volatile("invlpg (%0)" ::"r" (addr) : "memory");
    }
}