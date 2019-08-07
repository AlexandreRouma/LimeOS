#pragma once

#define PCI_MCTL_GET_DEVICE_COUNT   0
#define PCI_MCTL_GET_DEVICES        1
#define PCI_MCTL_GET_DEVICE         2

#define PCI_CLASS_UNCLASSIFIED      0x00
#define PCI_CLASS_MASS_STORAGE      0x01
#define PCI_CLASS_NETWORK           0x02
#define PCI_CLASS_DISPLAY           0x03
#define PCI_CLASS_MULTIMEDIA        0x04
#define PCI_CLASS_MEMORY            0x05
#define PCI_CLASS_BRIDGE            0x06
#define PCI_CLASS_SIMPLE_COM        0x07
#define PCI_CLASS_BASE_PERIPHERAL   0x08
#define PCI_CLASS_INPUT_DEVICE      0x09
#define PCI_CLASS_DOCKING_STATION   0x0A
#define PCI_CLASS_PROCESSOR         0x0B
#define PCI_CLASS_SERIAL_BUS        0x0C
#define PCI_CLASS_WIRELESS          0x0D
#define PCI_CLASS_INTELLIGENT       0x0E
#define PCI_CLASS_SATCOM            0x0F
#define PCI_CLASS_ENCRYPTION        0x10
#define PCI_CLASS_SIGNAL_PROCESS    0x11
#define PCI_CLASS_PROCESSING_ACCEL  0x12
#define PCI_CLASS_NON_ESSENTIAL     0x13
#define PCI_CLASS_COPROCESSOR       0x40
#define PCI_CLASS_UNASSIGNED        0xFF

#define PCI_SUBCLASS_OTHER          0x80
#define PCI_SUBCLASS_NON_VGA        0x00
#define PCI_SUBCLASS_VGA            0x01
#define PCI_SUBCLASS_SCSI_BUS       0x00 
#define PCI_SUBCLASS_IDE            0x01
#define PCI_SUBCLASS_FLOPPY         0x02
#define PCI_SUBCLASS_IPI            0x03
#define PCI_SUBCLASS_RAID           0x04
#define PCI_SUBCLASS_ATA            0x05
#define PCI_SUBCLASS_SATA           0x06
#define PCI_SUBCLASS_SERIAL_SCSI    0x07
#define PCI_SUBCLASS_NON_VOL_MEM    0x08

// TODO: Define all other subclasses

struct PCIHeaderType_t {
    uint8_t type:7;
    uint8_t isMultiFunc:1;
};

struct PCIBist_t {
    uint8_t completionCode:4;
    uint8_t _reserved0:2;
    uint8_t startBist:1;
    uint8_t bistCap:1;
};

struct PCIMemBAR_t {
    uint32_t isIO:1;
    uint32_t type:2;
    uint32_t prefetchable:1;
    uint32_t baseAddr:28;
};
struct PCIIOBAR_t {
    uint32_t isIO:1;
    uint32_t _reserved0:1;
    uint32_t baseAddr:30;
};

struct PCIHeader_t {
    union {
        bool isIO:1;
        PCIMemBAR_t mem;
        PCIIOBAR_t io;
    } BAR0;
    union {
        bool isIO:1;
        PCIMemBAR_t mem;
        PCIIOBAR_t io;
    } BAR1;
    union {
        bool isIO:1;
        PCIMemBAR_t mem;
        PCIIOBAR_t io;
    } BAR2;
    union {
        bool isIO:1;
        PCIMemBAR_t mem;
        PCIIOBAR_t io;
    } BAR3;
    union {
        bool isIO:1;
        PCIMemBAR_t mem;
        PCIIOBAR_t io;
    } BAR4;
    union {
        bool isIO:1;
        PCIMemBAR_t mem;
        PCIIOBAR_t io;
    } BAR5;
    uint32_t cardbusCISPtr;
    uint16_t subsysVendorId;
    uint16_t subsysId;
    uint32_t expROMBase;
    uint8_t capPtr;
    uint8_t _reserved0[7];
    uint8_t intLine;
    uint8_t intPin;
    uint8_t minGrant;
    uint8_t maxLatency;
};

struct PCI2PCIHeader_t {
    union {
        bool isIO:1;
        PCIMemBAR_t mem;
        PCIIOBAR_t io;
    } BAR0;
    union {
        bool isIO:1;
        PCIMemBAR_t mem;
        PCIIOBAR_t io;
    } BAR1;
    uint8_t primBusNum;
    uint8_t secBusNum;
    uint8_t subBusNum;
    uint8_t secLatencyTimer;
    uint8_t ioBase;
    uint8_t ioLimit;
    uint16_t secStatus;
    uint16_t memBase;
    uint16_t memLimit;
    uint16_t prefetchMemBase;
    uint16_t prefetchMemLimit;
    uint32_t prefetchBaseUpper;
    uint32_t prefetchLimitUpper;
    uint16_t ioBaseUpper;
    uint16_t ioLimitUpper;
    uint8_t capPtr;
    uint8_t _reserved0[3];
    uint32_t expROMBase;
    uint8_t intLine;
    uint8_t intPin;
    uint16_t bridgeControl;
};

struct PCI2CardbusHeader_t {
    uint32_t socketBase;
    uint8_t capListOffset;
    uint8_t _reserved0;
    uint16_t secStatus;
    uint8_t pciBusNum;
    uint8_t cbBusNum;
    uint8_t subBusNum;
    uint8_t cbLatencyTimer;
    uint32_t memBaseAddr0;
    uint32_t memLimit0;
    uint32_t memBaseAddr1;
    uint32_t memLimit1;
    uint32_t ioBaseAddr0;
    uint32_t ioLimit0;
    uint32_t ioBaseAddr1;
    uint32_t ioLimit1;
    uint8_t intLine;
    uint8_t intPin;
    uint16_t bridgeControl;
    uint16_t subsysDevId;
    uint16_t subsysVendorId;
    uint32_t legacyBaseAddr;
};

struct PCIDevice_t {
    uint16_t vendorId;
    uint16_t deviceId;
    uint16_t command;
    uint16_t status;
    uint8_t revId;
    uint8_t profIF;
    uint8_t subclass;
    uint8_t classCode;
    uint8_t cacheLineSize;
    uint8_t latencyTimer;
    PCIHeaderType_t headerType;
    PCIBist_t BIST;
    union {
        PCIHeader_t base;
        PCI2PCIHeader_t pci2pci;
        PCI2CardbusHeader_t pci2cardbus;
    } header;
    
}__attribute__((packed));

// MCTL

struct PCIDeviceAddr_t {
    uint8_t bus;
    uint8_t slot;
    uint8_t func;
};

struct PCIDeviceInfo_t {
    PCIDeviceAddr_t addr;
    PCIDevice_t device;
};