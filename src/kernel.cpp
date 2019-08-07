#include <init/init.h>
#include <multiboot/multiboot.h>
#include <misc/liballoc.h>
#include <vfs/vfs.h>
#include <vfs/fileio.h>
#include <temp_vga/terminal.h>
#include <stream.h>
#include <vector.h>
#include <ramfs/tarfs.h>
#include <paging/paging.h>
#include <gdt/gdt.h>
#include <panic.h>
#include <syscalls.h>
#include <kernio/kernio.h>
#include <vfs/miscstreams.h>
#include <kscript/kscript.h>
#include <kmod/modules.h>
#include <kmod/mctl.h>
#include <misc/cpuio.h>
#include <scheduler/scheduler.h>
#include <interrupts/idt.h>
#include <multiboot/ksyms.h>
#include <misc/systicks.h>
#include <stdctl/pci.h>
#include <elf/elfparser.h>

#define VERSION_STR "1.8.0"

#ifndef ARCH_STR
#define ARCH_STR "unknown"
#endif

#define BochsBreak() outw(0x8A00,0x8A00); outw(0x8A00,0x08AE0);

typedef volatile struct _AHCI_FIS_REG_H2D_t {
	uint8_t fis_type;

	uint8_t pmport:4;
	uint8_t _reserved0:3;
	uint8_t c:1;

	uint8_t command;
	uint8_t featurel;

    uint32_t lbal:24;
    uint32_t device:8;

    uint32_t lbah:24;
    uint32_t featureh:8;

	uint16_t count;
	uint8_t  icc;
	uint8_t  control;

	uint8_t  _reserved1[4];
}AHCI_FIS_REG_H2D_t;

typedef volatile struct _AHCI_FIS_REG_D2H_t {
	uint8_t fis_type;

	uint8_t pmport:4;
	uint8_t _reserved0:2;
    uint8_t i:1;
	uint8_t _reserved1:1;

	uint8_t status;
	uint8_t error;

    uint32_t lbal:24;
    uint32_t device:8;

    uint32_t lbah:24;
    uint32_t _reserved2:8;

	uint16_t count;
	uint16_t  _reserved3;

	uint8_t  _reserved4[4];
}AHCI_FIS_REG_D2H_t;

typedef volatile struct _AHCI_FIS_DATA_t {
    uint8_t fis_type;

	uint8_t pmport:4;
	uint8_t _reserved0:4;
    uint16_t _reserved1;

    uint32_t data[1];
}AHCI_FIS_DATA_t;

typedef volatile struct _AHCI_FIS_PIO_SETUP_t {
	uint8_t fis_type;
 
	uint8_t pmport:4;
	uint8_t _reserved0:1;
	uint8_t d:1;
	uint8_t i:1;
	uint8_t _reserved1:1;
 
	uint8_t status;
	uint8_t error;
 
	uint32_t lbal:24;
    uint32_t device:8;
 
	uint32_t lbah:24;
    uint32_t _reserved2:8;
 
	uint16_t count;
	uint8_t _reserved3;
	uint8_t e_status;
 
	uint16_t tc;
	uint16_t _reserved4;
}AHCI_FIS_PIO_SETUP_t;

typedef volatile struct _AHCI_FIS_DMA_SETUP_t {
	uint8_t fis_type;
 
	uint8_t pmport:4;
	uint8_t _reserved0:1;
	uint8_t d:1;
	uint8_t i:1;
	uint8_t a:1;
 
    uint16_t _reserved1;

    uint64_t DMAbufferID;

    uint32_t _reserved2;

    uint32_t DMAbufOffset;

    uint32_t TransferCount;

    uint32_t _reserved3;
}AHCI_FIS_DMA_SETUP_t;

typedef volatile struct _AHCI_HBA_PORT_t {
	uint64_t clb;
	uint32_t fb;
	uint32_t fbu;
	volatile uint32_t is;
	volatile uint32_t ie;
	volatile uint32_t cmd;
	uint32_t _reserved0;
	uint32_t tfd;
	uint32_t sig;
	volatile uint32_t ssts;
	uint32_t sctl;
	volatile uint32_t serr;
	uint32_t sact;
	volatile uint32_t ci;
	uint32_t sntf;
	uint32_t fbs;
	uint32_t _reserved1[11];
	uint32_t vendor[4];
}AHCI_HBA_PORT_t;

typedef volatile struct _AHCI_HBA_MEM_t {
	uint32_t cap;
	uint32_t ghc;
	uint32_t is;
	uint32_t pi;
	uint32_t vs;
	uint32_t ccc_ctl;
	uint32_t ccc_pts;
	uint32_t em_loc;
	uint32_t em_ctl;
	uint32_t cap2;
	uint32_t bohc;
 
	uint8_t _reserved0[0x74];
 
	uint8_t vendor[0x60];
 
	AHCI_HBA_PORT_t	ports[32];
}AHCI_HBA_MEM_t;

typedef volatile struct _ACHI_HBA_FIS_t {
	AHCI_FIS_DMA_SETUP_t dsfis;
	uint8_t _pad0[4];
 
	AHCI_FIS_PIO_SETUP_t psfis;
	uint8_t _pad1[12];
 
	AHCI_FIS_REG_D2H_t rfis;
	uint8_t _pad2[4];
 
	uint16_t sdbfis;
 
	uint8_t ufis[64];
 
	uint8_t _reserved0[0x60];
}ACHI_HBA_FIS_t;

typedef volatile struct _AHCI_HBA_CMD_HEADER_t {
	uint8_t cfl:5;
	uint8_t a:1;
	uint8_t w:1;
	uint8_t p:1;
 
	uint8_t r:1;
	uint8_t b:1;
	uint8_t c:1;
	uint8_t _reserved0:1;
	uint8_t pmp:4;
 
	uint16_t prdtl;
 
	volatile
	uint32_t prdbc;
 
	uint64_t ctba;
 
	uint32_t _reserved1[4];
}AHCI_HBA_CMD_HEADER_t;

typedef struct _AHCI_HBA_PRDT_ENTRY_t {
	uint64_t dba;
	uint32_t _reserved0;
 
	uint32_t dbc:22;
	uint32_t _reserved1:9;
	uint32_t i:1;
}AHCI_HBA_PRDT_ENTRY_t;

typedef volatile struct _AHCI_HBA_CMD_TBL_t {
	uint8_t cfis[64];
 
	uint8_t acmd[16];
 
	uint8_t _reserved0[48];
 
	AHCI_HBA_PRDT_ENTRY_t* prdt_entries;
}AHCI_HBA_CMD_TBL_t;

#define	SATA_SIG_ATA	        0x00000101	// SATA drive
#define	SATA_SIG_ATAPI	        0xEB140101	// SATAPI drive
#define	SATA_SIG_SEMB	        0xC33C0101	// Enclosure management bridge
#define	SATA_SIG_PM	            0x96690101	// Port multiplier
#define HBA_PORT_IPM_ACTIVE     1
#define HBA_PORT_DET_PRESENT    3

#define AHCI_FIS_TYPE_REG_H2D       0x27
#define AHCI_FIS_TYPE_REG_D2H       0x34
#define AHCI_FIS_TYPE_DMA_ACT       0x39
#define AHCI_FIS_TYPE_DMA_SETUP     0x41
#define AHCI_FIS_TYPE_DATA          0x46
#define AHCI_FIS_TYPE_BIST          0x58
#define AHCI_FIS_TYPE_PIO_SETUP     0x5F
#define AHCI_FIS_TYPE_DEV_BITS      0xA1

#define AHCI_HBA_GHC_AE             (1 << 0)

#define AHCI_HBA_PxCMD_ST           (1 << 0)
#define AHCI_HBA_PxCMD_SUD          (1 << 1)
#define AHCI_HBA_PxCMD_POD          (1 << 2)
#define AHCI_HBA_PxCMD_CLO          (1 << 3)
#define AHCI_HBA_PxCMD_FRE          (1 << 4)
#define AHCI_HBA_PxCMD_MPSS         (1 << 13)
#define AHCI_HBA_PxCMD_FR           (1 << 14)
#define AHCI_HBA_PxCMD_CR           (1 << 15)
#define AHCI_HBA_PxCMD_CPS          (1 << 16)
#define AHCI_HBA_PxCMD_PMA          (1 << 17)
#define AHCI_HBA_PxCMD_HPCP         (1 << 18)
#define AHCI_HBA_PxCMD_MPSP         (1 << 19)
#define AHCI_HBA_PxCMD_CPD          (1 << 20)
#define AHCI_HBA_PxCMD_ESP          (1 << 21)
#define AHCI_HBA_PxCMD_FBSCP        (1 << 22)
#define AHCI_HBA_PxCMD_APSTE        (1 << 23)
#define AHCI_HBA_PxCMD_ATAPI        (1 << 24)
#define AHCI_HBA_PxCMD_DLAE         (1 << 25)
#define AHCI_HBA_PxCMD_ALPE         (1 << 26)
#define AHCI_HBA_PxCMD_ASP          (1 << 27)

#define AHCI_HBA_PxIE_DHRE          (1 << 0)
#define AHCI_HBA_PxIE_PSE           (1 << 1)
#define AHCI_HBA_PxIE_DSE           (1 << 2)
#define AHCI_HBA_PxIE_SDBE          (1 << 3)
#define AHCI_HBA_PxIE_UFE           (1 << 4)
#define AHCI_HBA_PxIE_DPE           (1 << 5)
#define AHCI_HBA_PxIE_PCE           (1 << 6)
#define AHCI_HBA_PxIE_DPME          (1 << 7)
#define AHCI_HBA_PxIE_PRCE          (1 << 22)
#define AHCI_HBA_PxIE_IPME          (1 << 23)
#define AHCI_HBA_PxIE_OFE           (1 << 24)
#define AHCI_HBA_PxIE_INFE          (1 << 26)
#define AHCI_HBA_PxIE_IFE           (1 << 27)
#define AHCI_HBA_PxIE_HBDE          (1 << 28)
#define AHCI_HBA_PxIE_HBFE          (1 << 29)
#define AHCI_HBA_PxIE_TFEE          (1 << 30)
#define AHCI_HBA_PxIE_CPDE          (1 << 31)

#define AHCI_HBA_PxIS_DHRS          (1 << 0)
#define AHCI_HBA_PxIS_PSS           (1 << 1)
#define AHCI_HBA_PxIS_DSS           (1 << 2)
#define AHCI_HBA_PxIS_SDBS          (1 << 3)
#define AHCI_HBA_PxIS_UFS           (1 << 4)
#define AHCI_HBA_PxIS_DPS           (1 << 5)
#define AHCI_HBA_PxIS_PCS           (1 << 6)
#define AHCI_HBA_PxIS_DMPS          (1 << 7)
#define AHCI_HBA_PxIS_PRCS          (1 << 22)
#define AHCI_HBA_PxIS_IPMS          (1 << 23)
#define AHCI_HBA_PxIS_OFS           (1 << 24)
#define AHCI_HBA_PxIS_INFS          (1 << 26)        
#define AHCI_HBA_PxIS_IFS           (1 << 27)
#define AHCI_HBA_PxIS_HBDS          (1 << 28)
#define AHCI_HBA_PxIS_HBFS          (1 << 29)
#define AHCI_HBA_PxIS_TFES          (1 << 30)
#define AHCI_HBA_PxIS_CPDS          (1 << 31)

#define AHCI_HBA_PxTFD_STS_ERR      (1 << 0)
#define AHCI_HBA_PxTFD_STS_DRQ      (1 << 3)
#define AHCI_HBA_PxTFD_STS_BSY      (1 << 7)


#define AHCI_DMA_SETUP              (1 << 2)
#define AHCI_DHR_SETUP              (1 << 0)


// ATA COMMANDS, NOTHING TO DO WITH AHCI

#define ATA_SR_BSY     0x80    // Busy
#define ATA_SR_DRDY    0x40    // Drive ready
#define ATA_SR_DF      0x20    // Drive write fault
#define ATA_SR_DSC     0x10    // Drive seek complete
#define ATA_SR_DRQ     0x08    // Data request ready
#define ATA_SR_CORR    0x04    // Corrected data
#define ATA_SR_IDX     0x02    // Index
#define ATA_SR_ERR     0x01    // Error

#define ATA_ER_BBK      0x80    // Bad block
#define ATA_ER_UNC      0x40    // Uncorrectable data
#define ATA_ER_MC       0x20    // Media changed
#define ATA_ER_IDNF     0x10    // ID mark not found
#define ATA_ER_MCR      0x08    // Media change request
#define ATA_ER_ABRT     0x04    // Command aborted
#define ATA_ER_TK0NF    0x02    // Track 0 not found
#define ATA_ER_AMNF     0x01    // No address mark

#define ATA_CMD_READ_PIO          0x20
#define ATA_CMD_READ_PIO_EXT      0x24
#define ATA_CMD_READ_DMA          0xC8
#define ATA_CMD_READ_DMA_EXT      0x25
#define ATA_CMD_WRITE_PIO         0x30
#define ATA_CMD_WRITE_PIO_EXT     0x34
#define ATA_CMD_WRITE_DMA         0xCA
#define ATA_CMD_WRITE_DMA_EXT     0x35
#define ATA_CMD_CACHE_FLUSH       0xE7
#define ATA_CMD_CACHE_FLUSH_EXT   0xEA
#define ATA_CMD_PACKET            0xA0
#define ATA_CMD_IDENTIFY_PACKET   0xA1
#define ATA_CMD_IDENTIFY          0xEC

#define ATAPI_CMD_READ       0xA8
#define ATAPI_CMD_EJECT      0x1B



void listNodes(char* path) {
    kio::print("Listing ");
    kio::println(path);
    if (vfs::nodeExists(path)) {
        vector<FSNode_t> nodes = vfs::listNodes(path);
        for (int i = 0; i < nodes.size(); i++) {
            if (nodes[i].flags & FS_FLAG_D) {
                kio::setFore(0x09);
            }
            else {
                kio::setFore(0x07);
            }
            kio::print("> ");
            kio::println(nodes[i].name);
        }
        kio::println("");
        kio::setFore(0x0F);
    }
    else {
        kio::print("No such directory: ");
        kio::println(path);
    }
    
}

void printFile(char* path) {
    if (vfs::nodeExists(path)) {
        stream_t s = vfs::getStream(path);
        if (s.slen > 0) {
            kio::printf("Printing '%s':\n", path);
            char* buf = (char*)malloc(s.slen + 1);
            buf[s.slen] = 0;
            stream::read(s, buf, s.slen);
            kio::print(buf);
            free(buf);
        }
        stream::close(s);
    }
    else {
        kio::print("No such file: ");
        kio::println(path);
    }
}

void lspci() {
    vector<FSNode_t> devices = vfs::listNodes("/fio/dev/pci");
    for (int i = 0; i < devices.size(); i++) {
        stream_t s = vfs::getStream((string("/fio/dev/pci/") + devices[i].name).c_str());
        if (s.slen > 0) {
            char* buf = (char*)malloc(s.slen + 1);
            buf[s.slen] = 0;
            stream::read(s, buf, s.slen);
            kio::printf("[%s] %s\n", devices[i].name, buf);
            free(buf);
        }
        stream::close(s);
    }
}

void AHCIResetPort(volatile AHCI_HBA_PORT_t* port) {
    port->cmd &= ~AHCI_HBA_PxCMD_ST;
    uint64_t ticks = systicks::getTicks() + 500;
    while (port->cmd & AHCI_HBA_PxCMD_CR) {
        if (ticks < systicks::getTicks()) {
            break; // port is hung :/
        }
    }
    port->sctl |= 1; // write 1 to det
    scheduler::sleep(10);
    port->sctl &= ~0x000F; // write 0 to det

    while (port->sctl & 0x000F != 3); // Todo: HBA reset if not working

    port->serr = ~0; // Clear errors
}

bool AHCIStopPort(volatile AHCI_HBA_PORT_t* port) {
    for (int i = 0; i < 2; i++) {
        port->cmd &= ~AHCI_HBA_PxCMD_ST;
        port->cmd &= ~AHCI_HBA_PxCMD_FRE;
        uint64_t ticks = systicks::getTicks() + 500;
        while (port->cmd & (AHCI_HBA_PxCMD_CR | AHCI_HBA_PxCMD_FRE)) {
            if (ticks < systicks::getTicks()) {
                AHCIResetPort(port);
                continue;
            }
        }
        return true;
    }
    return false;
}

bool AHCIStartPort(volatile AHCI_HBA_PORT_t* port) {
    for (int i = 0; i < 2; i++) {
        port->serr |= (1 << 26); // Set DIAG.X
        uint64_t ticks = systicks::getTicks() + 500;
        //(port->ssts & 0xF) != 3 || (port->tfd & AHCI_HBA_PxTFD_STS_BSY) != 0 || (port->tfd & AHCI_HBA_PxTFD_STS_DRQ) != 0
        while (port->cmd & AHCI_HBA_PxCMD_CR) {
            if (ticks < systicks::getTicks()) {
                AHCIResetPort(port);
                continue;
            }
        }
        port->cmd |= AHCI_HBA_PxCMD_ST;
        port->cmd |= AHCI_HBA_PxCMD_FRE;
        return true;
    }
    return false;
}

extern "C"
void _kmain(uint32_t multiboot_magic, multiboot_info* multiboot_info) {
    msb::boot_info = multiboot_info;
    _init(multiboot_info);

    ksyms::load(multiboot_info);

    // Init VFS
    vfs::init();

    // Init fileio
    fio::init();
    miscstrms::init();

    // Module Control
    mctl::init();

    // Redirect kernel stdout
    kio::stdout = vfs::getStream("/fio/dev/null");

    // Load ram filesystem
    int id = -1;
    multiboot_module_t* mods = (multiboot_module_t*)multiboot_info->mods_addr;
    for (int i = 0; i < multiboot_info->mods_count; i++) {
        if (strcmp((char*)mods[i].cmdline, "ramfs")) {
            id = i;
        }
    }
    if (id == -1) {
        kpanic("Couldn't load RAMFS", 0);
    }
    tarfs::init(mods[id].mod_start, "/");

    // Init module interface
    kmod::init(multiboot_info);

    // run kscript
    ksc::init();
    ksc::run("/conf/init.ksc");

    kio::printf("\n\n");

    kio::println("LimeOS Kernel v" VERSION_STR "-" ARCH_STR " (Compiler version: " __VERSION__ ")\n\n");

    lspci();

    kio::printf("Using %u bytes\n", paging::getUsedPages() * 4096);

    kio::print(LOG_UNDEF "Searching for an AHCI controller...");

    uint32_t count = 0;
    mctl::call("/fio/dev/pci", PCI_MCTL_GET_DEVICE_COUNT, NULL, &count);
    
    PCIDeviceInfo_t* devices = (PCIDeviceInfo_t*)malloc(sizeof(PCIDeviceInfo_t) * count);
    mctl::call("/fio/dev/pci", PCI_MCTL_GET_DEVICES, NULL, devices);

    bool found = false;
    PCIDevice_t ahciController;
    PCIDeviceAddr_t controllerAddr;
    for (int i = 0; i < count; i++) {
        if (devices[i].device.classCode == PCI_CLASS_MASS_STORAGE && devices[i].device.subclass == PCI_SUBCLASS_SATA) {
            ahciController = devices[i].device;
            controllerAddr = devices[i].addr;
            found = true;
            kio::ok();
        }
    }
    if (!found) {
        kio::failed();
        kio::setFore(ANSI_COL_RED);
        kio::println("Oopsie Woopsie!!! UwU we did a fucky wucky! A wittke fucko boigo...\n The code monkeys at out headquarters are working vewy hawd to fix this UwU");
        while (1);
    }
    free(devices);

    found = false;
    uint32_t ABAR = ahciController.header.base.BAR5.mem.baseAddr << 4;
    AHCI_HBA_MEM_t* hbaMem = (AHCI_HBA_MEM_t*)ABAR;
    paging::setPresent(ABAR, paging::sizeToPages(sizeof(AHCI_HBA_MEM_t)));
    uint32_t portN = 0;

    // 1) Indicated software is AHCI aware
    hbaMem->ghc  |= AHCI_HBA_GHC_AE;

    // 2) Determin which ports are implemented
    uint32_t pi = hbaMem->pi;
    for (int i = 0; i < 32; i++) {
        if ((hbaMem->pi >> i) & 1) {
            uint32_t ssts = hbaMem->ports[i].ssts;
            uint32_t sig = hbaMem->ports[i].sig;
            uint8_t ipm = (ssts >> 8) & 0x0F;
	        uint8_t det = ssts & 0x0F;
            if (ipm != HBA_PORT_IPM_ACTIVE || det != HBA_PORT_DET_PRESENT) {
                continue;
            }

            if (sig == SATA_SIG_ATA) {
                kio::printf("Found a SATA drive on port %u\n", i);
            }
            else if (sig == SATA_SIG_ATAPI) {
                kio::printf("Found an ATAPI drive on port %u\n", i);
            }
            else if (sig == SATA_SIG_PM) {
                kio::printf("Found a port multiplier on port %u\n", i);
            }
            else if (sig == SATA_SIG_SEMB) {
                kio::printf("Found a SEMB drive on port %u\n", i);
            }
            else {
                kio::printf("Found unknown type drive on port %u\n", i);
            }
            found = true;
            portN = i;
            break;
        }
    }

    kio::println("Port scan done!");

    AHCI_HBA_PORT_t* port = &hbaMem->ports[portN];

    // 3) Ensure port is NOT running
    if (port->cmd & (AHCI_HBA_PxCMD_ST | AHCI_HBA_PxCMD_CR | AHCI_HBA_PxCMD_FRE | AHCI_HBA_PxCMD_FR)) {
        kio::println("Port is NOT idle, stopping port...");
        AHCIStopPort(port);
    }
    else {
        kio::println("HBA is idle!");
    }

    uint16_t ncs = (hbaMem->cap >> 8) & 0x1F;

    kio::printf("cap.ncs: 0x%08X\n", ncs);

    port->clb = paging::allocPages(1);
    port->fb = paging::allocPages(1);
    uint32_t cmdTbl = paging::allocPages(0x80 + (sizeof(AHCI_HBA_PRDT_ENTRY_t) * 256)); // 256 = prdt entry count

    paging::setFlags(port->clb, 1, PT_PRESENT | PT_RW | PT_CACHE_D);
    paging::setFlags(port->fb, 1, PT_PRESENT | PT_RW | PT_CACHE_D);
    paging::setFlags(cmdTbl, 1, PT_PRESENT | PT_RW | PT_CACHE_D);
    
    memset((void*)port->clb, 0, 4096);
    memset((void*)port->fb, 0, 4096);
    memset((void*)cmdTbl, 0, 0x80 + (sizeof(AHCI_HBA_PRDT_ENTRY_t) * 256));
    AHCI_HBA_CMD_HEADER_t* hdrs = (AHCI_HBA_CMD_HEADER_t*)port->clb;
    for (int i = 0; i < ncs; i++) {
        hdrs[i].ctba = cmdTbl;
    }

    // Wait for device to be ready before enabling
    // ssts.det != 3,  tfd.sts.bsy != 0 , stf.sts.drq != 0

    port->ie |= AHCI_DHR_SETUP;
    port->ie |= AHCI_DMA_SETUP;

    kio::println("Starting port...");
    AHCIStartPort(port);
    kio::println("Port started!");

    scheduler::sleep(500);


    int slot = 0;

    int identifyCmdSize = 512;
    uint16_t* buffer = (uint16_t*)malloc(identifyCmdSize);
    memset(buffer, 0, identifyCmdSize);

    AHCI_HBA_CMD_HEADER_t* headers = (AHCI_HBA_CMD_HEADER_t*)port->clb;
    AHCI_HBA_CMD_HEADER_t* cmdHeader = &headers[slot];
    cmdHeader->cfl = sizeof(AHCI_FIS_REG_H2D_t)/sizeof(uint32_t);
    cmdHeader->w = 0; // No write
    cmdHeader->prdtl = 1; // Only one PRDT entry needed;
    //cmdHeader->a = 0; // No atapi

    AHCI_HBA_CMD_TBL_t* commandTbl = (AHCI_HBA_CMD_TBL_t*)cmdHeader->ctba;
	commandTbl->prdt_entries[0].dbc = identifyCmdSize - 1;
    commandTbl->prdt_entries[0].dba = (uint64_t)buffer;
    commandTbl->prdt_entries[0].i = 1;

    AHCI_FIS_REG_H2D_t* cmdFIS = (AHCI_FIS_REG_H2D_t*)&commandTbl->cfis[0];
    memset((void*)cmdFIS, 0, sizeof(AHCI_FIS_REG_H2D_t));
    cmdFIS->fis_type = AHCI_FIS_TYPE_REG_H2D;
    cmdFIS->c = 1;
    cmdFIS->command = ATA_CMD_IDENTIFY;

    // WAit for device
    kio::println("Waiting for device to be ready");
    while (port->tfd & (AHCI_HBA_PxTFD_STS_BSY | AHCI_HBA_PxTFD_STS_DRQ));
    kio::println("DEVICE READY! Issuing command...");

    port->is = ~0;
    port->ci = (1 << slot);

    // FLUSH 
    volatile uint32_t c = port->ci;
    c=c;
    // FLUSH 

    bool error = false;
    int loops = 0;
    while (1)
	{
		// In some longer duration reads, it may be helpful to spin on the DPS bit 
		// in the PxIS port field as well (1 << 5)
		if ((port->ci & (1<<slot)) == 0) 
			break;
		if (port->is & AHCI_HBA_PxIS_TFES)	// Task file error
		{
			error = true;
			break;
		}
        loops++;
	}

    if (error) {
        kio::printf("OH GOD OH FUCK DRIVE ERROR! 0x%08X\n", port->ssts);
    }
    else {
        kio::println("Command done!");
    }

    kio::printf("%04X %04X %04X %04X", buffer[100], buffer[101], buffer[102], buffer[103]);



    // NOTE: This loop is for the scheduler to fall back to when no task is currently running
    uint32_t uptime = 1;
    while(1) {
        
    }
}