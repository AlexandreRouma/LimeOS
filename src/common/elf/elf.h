#pragma once
#include <stdint.h>

#define ELF_ET_NONE         0x0
#define ELF_ET_REL          0x1
#define ELF_ET_EXEC         0x2
#define ELF_ET_DYN          0x3
#define ELF_ET_CORE         0x4
#define ELF_ET_LOPROC       0xFF00
#define ELF_ET_HIPROC       0xFFFF

#define ELF_EM_NONE         0x0
#define ELF_EM_M32          0x1
#define ELF_EM_SPARC        0x2
#define ELF_EM_386          0x3
#define ELF_EM_68K          0x4
#define ELF_EM_88K          0x5
#define ELF_EM_860          0x7
#define ELF_EM_MIPS         0x8

#define ELF_EV_NONE         0x0
#define ELF_EV_CURRENT      0x1

#define ELF_BITSNONE        0x0
#define ELF_BITS32          0x1
#define ELF_BITS64          0x2

#define ELF_DATANONE        0x0
#define ELF_DATALSB         0x1
#define ELF_DATAMSB         0x2

#define ELF_SHT_NULL        0x0
#define ELF_SHT_PROGBITS    0x1
#define ELF_SHT_SYMTAB      0x2
#define ELF_SHT_STRTAB      0x3
#define ELF_SHT_RELA        0x4
#define ELF_SHT_HASH        0x5
#define ELF_SHT_DYNAMIC     0x6
#define ELF_SHT_NOTE        0x7
#define ELF_SHT_NOBITS      0x8
#define ELF_SHT_REL         0x9
#define ELF_SHT_SHLIB       0xA
#define ELF_SHT_DYNSYM      0xB
#define ELF_SHT_LOPROC      0x70000000
#define ELF_SHT_HIPROC      0x7FFFFFFF
#define ELF_SHT_LOUSER      0x80000000
#define ELF_SHT_HIUSER      0x8FFFFFFF

#define ELF_SHF_NONE        0x0
#define ELF_SHF_WRITE       0x1
#define ELF_SHF_ALLOC       0x2
#define ELF_SHF_EXECINSTR   0x4
#define ELF_SHF_MASKPROC    0xF0000000

#define ELF_STB_LOCAL       0x0
#define ELF_STB_GLOBAL      0x1
#define ELF_STB_WEAK        0x2
#define ELF_STB_LOPROC      0x13
#define ELF_STB_HIPROC      0x15

struct ELFHeader_t {
    uint32_t magic;
    uint8_t bits;
    uint8_t endianess;
    uint8_t elfHeaderVersion;
    uint8_t OSABI;
    uint8_t _unused0[8];
    uint16_t type;
    uint16_t arch;
    uint32_t elfVersion;
    uint32_t progEntryPos;
    uint32_t progHeaderTablePos;
    uint32_t sectionHeaderTablePos;
    uint32_t flags;
    uint16_t headerSize;
    uint16_t progHeaderTableEntrySize;
    uint16_t progHeaderTableEntryCount;
    uint16_t sectionHeaderTableEntrySize;
    uint16_t sectionHeaderTableEntryCount;
    uint16_t sectionNameIndex;
}__attribute__((packed));

struct ELFSectionHeader_t {
    uint32_t name;
    uint32_t type;
    uint32_t flags;
    uint32_t addr;
    uint32_t offset;
    uint32_t size;
    uint32_t link;
    uint32_t info;
    uint32_t align;
    uint32_t entrySize;
}__attribute__((packed));

struct ELFSymbolTableEntry_t {
    uint32_t name;
    uint32_t addr;
    uint32_t size;
    uint8_t info;
    uint8_t other;
    uint16_t sectionHeaderIndex;
}__attribute__((packed));

struct ELFProgramHeader_t {
    uint32_t type;
    uint32_t p_offset;
    uint32_t p_vaddr;
    uint32_t _unused0;
    uint32_t p_filesz;
    uint32_t p_memsz;
    uint32_t flags;
    uint32_t align;
}__attribute__((packed));