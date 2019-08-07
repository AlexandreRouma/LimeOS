#pragma once
#include <stdint.h>

#define ELF_ET_NONE         0
#define ELF_ET_REL          1
#define ELF_ET_EXEC         2
#define ELF_ET_DYN          3
#define ELF_ET_CORE         4
#define ELF_ET_LOPROC       0xFF00
#define ELF_ET_HIPROC       0xFFFF

#define ELF_EM_NONE         0
#define ELF_EM_M32          1
#define ELF_EM_SPARC        2
#define ELF_EM_386          3
#define ELF_EM_68K          4
#define ELF_EM_88K          5
#define ELF_EM_860          7
#define ELF_EM_MIPS         8

#define ELF_EV_NONE         0
#define ELF_EV_CURRENT      1

#define ELF_BITSNONE        0
#define ELF_BITS32          1
#define ELF_BITS64          2

#define ELF_DATANONE        0
#define ELF_DATALSB         1
#define ELF_DATAMSB         2

#define ELF_SHT_NULL        0
#define ELF_SHT_PROGBITS    1
#define ELF_SHT_SYMTAB      2
#define ELF_SHT_STRTAB      3
#define ELF_SHT_RELA        4
#define ELF_SHT_HASH        5
#define ELF_SHT_DYNAMIC     6
#define ELF_SHT_NOTE        7
#define ELF_SHT_NOBITS      8
#define ELF_SHT_REL         9
#define ELF_SHT_SHLIB       10
#define ELF_SHT_DYNSYM      11
#define ELF_SHT_LOPROC      0x70000000
#define ELF_SHT_HIPROC      0x7FFFFFFF
#define ELF_SHT_LOUSER      0x80000000
#define ELF_SHT_HIUSER      0x8FFFFFFF

#define ELF_SHF_NONE        0
#define ELF_SHF_WRITE       1
#define ELF_SHF_ALLOC       2
#define ELF_SHF_EXECINSTR   4
#define ELF_SHF_MASKPROC    0xF0000000

#define ELF_STB_LOCAL       0
#define ELF_STB_GLOBAL      1
#define ELF_STB_WEAK        2
#define ELF_STB_LOPROC      13
#define ELF_STB_HIPROC      15

#define ELF_STT_NOTYPE      0
#define ELF_STT_OBJECT      1
#define ELF_STT_FUNC        2
#define ELF_STT_SECTION     3
#define ELF_STT_FILE        4
#define ELF_STT_LOPROC      13
#define ELF_STT_HIPROC      15

#define ELF_R_SYM(i)    ((i)>>8)
#define ELF_R_TYPE(i)   ((uint8_t)(i))

#define ELF_R_386_NONE      0
#define ELF_R_386_332       1
#define ELF_R_386_PC32      2
#define ELF_R_386_GOT32     3
#define ELF_R_386_PLT32     4
#define ELF_R_386_COPY      5
#define ELF_R_386_GLOB_DAT  6
#define ELF_R_386_JMP_SLOT  7
#define ELF_R_386_RELATIVE  8
#define ELF_R_386_GOTOFF    9
#define ELF_R_386_GOTPC     10

#define ELF_PT_NULL         0
#define ELF_PT_LOAD         1
#define ELF_PT_DYNAMIC      2
#define ELF_PT_INTERP       3
#define ELF_PT_NOTE         4
#define ELF_PT_SHLIB        5
#define ELF_PT_PHDR         6
#define ELF_PT_LOPROC       0x70000000
#define ELF_PT_HIPROC       0x7FFFFFFF


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

struct ELFSectionHeaderEntry_t {
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

struct ELFProgramHeaderEntry_t {
    uint32_t type;
    uint32_t p_offset;
    uint32_t p_vaddr;
    uint32_t _unused0;
    uint32_t p_filesz;
    uint32_t p_memsz;
    uint32_t flags;
    uint32_t align;
}__attribute__((packed));

struct ELFReloc_t {
    uint32_t addr;
    uint32_t info;
}__attribute__((packed));

struct ELFRelocAddend_t {
    uint32_t addr;
    uint32_t info;
    uint32_t addend;
}__attribute__((packed));

struct ELFDynStruct_t {
    uint32_t tag;
    uint32_t val;
}__attribute__((packed));