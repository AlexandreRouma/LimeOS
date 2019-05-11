#include <elf/elfldr.h>
#include <elf/elf.h>
#include <kernio/kernio.h>

ELFExec::ELFExec(uint32_t addr) {
    header = (ELFHeader_t*)addr;
    sectionHeader = (ELFSectionHeader_t*)(header->sectionHeaderTablePos + addr);
    SHEntryCount = header->sectionHeaderTableEntryCount;
    kio::printf("Section count: %u\n", header->sectionHeaderTableEntryCount);
    SHStringTable = (char*)(sectionHeader[header->sectionNameIndex].offset + addr);
}

vector<ELFSection_t> ELFExec::getSections() {
    vector<ELFSection_t> v;
    for (int i = 0; i < SHEntryCount; i++) {
        ELFSection_t s;
        s.addr = SHStringTable[sectionHeader[i].name];
        v.push_back(s);
    }
    return v;
}