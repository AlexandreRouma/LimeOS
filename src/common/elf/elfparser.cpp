#include <elf/elfparser.h>
#include <stream.h>
#include <vfs/vfs.h>
#include <paging/paging.h>
#include <kernio/kernio.h>

namespace elfparser {
    ELFFile_t load(char* path) {
        // Load file
        ELFFile_t file;
        stream_t s = vfs::getStream(path);
        file.offset = (uint32_t)malloc(s.slen);
        file.data = (char*)file.offset;
        stream::read(s, file.data, s.slen);
        stream::close(s);

        file.header = (ELFHeader_t*)file.data;
        file.sectionHeader.sections = (ELFSectionHeaderEntry_t*)(file.header->sectionHeaderTablePos + file.offset);
        file.sectionHeader.entryCount = file.header->sectionHeaderTableEntryCount;
        file.sectionHeader.stringTable = (char*)(file.sectionHeader.sections[file.header->sectionNameIndex].offset + file.offset);

        file.programHeader.entryCount = file.header->progHeaderTableEntryCount;
        file.programHeader.entries = (ELFProgramHeaderEntry_t*)(file.header->progHeaderTablePos + file.offset);

        for (int i = 0; i < file.sectionHeader.entryCount; i++) {
            if (file.sectionHeader.sections[i].type == ELF_SHT_SYMTAB || file.sectionHeader.sections[i].type == ELF_SHT_DYNSYM) {
                ELFSymbolTable_t table;
                table.sectionIndex = i;
                table.type = file.sectionHeader.sections[i].type;
                table.name = ELFGetString(file.sectionHeader.stringTable, file.sectionHeader.sections[i].name);
                table.entryCount = file.sectionHeader.sections[i].size / sizeof(ELFSymbolTableEntry_t);
                table.symbols = (ELFSymbolTableEntry_t*)(file.sectionHeader.sections[i].offset + file.offset);
                table.stringTable = (char*)(file.sectionHeader.sections[file.sectionHeader.sections[i].link].offset + file.offset);
                file.symbolTables.push_back(table);
            }
            else if (file.sectionHeader.sections[i].type == ELF_SHT_REL || file.sectionHeader.sections[i].type == ELF_SHT_RELA) {
                ELFRelocTable_t table;
                table.sectionIndex = i;
                table.type = file.sectionHeader.sections[i].type;
                table.relocs = (ELFReloc_t*)(file.sectionHeader.sections[i].offset + file.offset);
                table.addendRelocs = (ELFRelocAddend_t*)(file.sectionHeader.sections[i].offset + file.offset);
                table.symTab = file.sectionHeader.sections[i].link;
                table.affected = file.sectionHeader.sections[i].info;
                if (file.sectionHeader.sections[i].type == ELF_SHT_RELA) {
                    table.entryCount = file.sectionHeader.sections[i].size / sizeof(ELFRelocAddend_t);
                }
                else {
                    table.entryCount = file.sectionHeader.sections[i].size / sizeof(ELFReloc_t);
                }
                file.relocTables.push_back(table);
            }
        }
        return file;
    }

    int getSymbolTableId(ELFFile_t& file, int sectionId) {
        int sz = file.symbolTables.size();
        for (int i = 0; i < sz; i++) {
            if (file.symbolTables[i].sectionIndex == sectionId) {
                return i;
            }
        }
        return -1;
    }
}