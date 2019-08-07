#include <misc/memory.h>
#include <string.h>
#include <misc/cpuio.h>
#include <stream.h>
#include <vfs/vfs.h>
#include <vfs/fileio.h>
#include <kernio/kernio.h>
#include <stdctl/pci.h>
#include <pcivendors.hpp>
#include <map.h>
#include <kmod/mctl.h>

struct PCIDevID_t {
    uint8_t bus;
    uint8_t slot;
    uint8_t func;

    bool operator ==(PCIDevID_t& dev) {
        return dev.bus == bus && dev.slot == slot && dev.func == func;
    }
};

//vector<PCIDevice_t> fioDevices;

uint8_t readConfigByte(uint32_t bus, uint32_t slot, uint32_t func, uint32_t offset) {
    uint32_t addr = ((uint32_t)1 << 31) | (bus << 16) | ((slot & 0x1F) << 11) | ((func & 7) << 8) | (offset & 0xFC);
    outl(0xCF8, addr);
    uint32_t res = inl(0xCFC);
    return res >> ((offset & 3) * 8);
}

uint16_t readConfigWord(uint32_t bus, uint32_t slot, uint32_t func, uint32_t offset) {
    uint32_t addr = ((uint32_t)1 << 31) | (bus << 16) | ((slot & 0x1F) << 11) | ((func & 7) << 8) | (offset & 0xFC);
    outl(0xCF8, addr);
    uint32_t res = inl(0xCFC);
    return res >> ((offset & 2) * 8);
}

uint32_t readConfigLong(uint32_t bus, uint32_t slot, uint32_t func, uint32_t offset) {
    uint32_t addr = (1 << 31) | (bus << 16) | ((slot & 0x1F) << 11) | ((func & 7) << 8) | (offset & 0xFC);
    outl(0xCF8, addr);
    return inl(0xCFC);
}

PCIDevice_t getDevice(uint8_t bus, uint8_t slot, uint8_t func) {
    PCIDevice_t dev;
    uint32_t* buf = (uint32_t*)&dev;
    for (int i = 0; i < 16; i++) {
        buf[i] = readConfigLong(bus, slot, func, i * 4);
    }
    return dev;
}

char* findVendorString(uint16_t vendor) {
    for (int i = 0; i < PCI_VENTABLE_LEN; i++) {
        if (PciVenTable[i].VenId == vendor) {
            return PciVenTable[i].VenFull;
        }
    }
    return "Unknown";
}

char* findDeviceString(uint16_t vendor, uint16_t device) {
    for (int i = 0; i < PCI_DEVTABLE_LEN; i++) {
        if (PciDevTable[i].VenId == vendor && PciDevTable[i].DevId == device) {
            return PciDevTable[i].Chip;
        }
    }
    return "Unknown";
}

char* findDeviceDesc(uint16_t vendor, uint16_t device) {
    for (int i = 0; i < PCI_DEVTABLE_LEN; i++) {
        if (PciDevTable[i].VenId == vendor && PciDevTable[i].DevId == device) {
            return PciDevTable[i].ChipDesc;
        }
    }
    return "Unknown";
}

char* findDeviceClass(uint8_t cl) {
    for (int i = 0; i < PCI_CLASSCODETABLE_LEN; i++) {
        if (PciClassCodeTable[i].BaseClass == cl) {
            return (char*)PciClassCodeTable[i].BaseDesc;
        }
    }
    return "Unknown";
}

char* findDeviceSubclass(uint8_t cl,  uint8_t subcl) {
    for (int i = 0; i < PCI_CLASSCODETABLE_LEN; i++) {
        if (PciClassCodeTable[i].BaseClass == cl && PciClassCodeTable[i].SubClass == subcl) {
            return (char*)PciClassCodeTable[i].SubDesc;
        }
    }
    return "Unknown";
}

vector<PCIDeviceInfo_t> fioDevices;

uint32_t _writeHndlr(stream_t s, uint32_t len, uint64_t pos) {
    return 0;
}

uint32_t _readHndlr(stream_t s, uint32_t len, uint64_t pos) {
    // TODO: Security breach, correct this so that programs can't read anywhere in ram
    memcpy(s.buffer, s.tag + pos, len);
    return len;
}

void _closeHndlr(stream_t s) {
    free(s.tag);
}

stream_t _Provider(void* tag) {
    PCIDevice_t dev = fioDevices[(uint32_t)tag].device;
    // VEN_ID;DEV_ID;CLASS;SUBCLASS;VENDOR_TXT;CHIP_TXT;DESC_TXT;CLASS_TXT;SUBCLASS_TXT
    string str = sprintf("%04X;%04X;%02X;%02X;%s;%s;%s;%s;%s", dev.vendorId, dev.deviceId, dev.classCode, dev.subclass, 
                        findVendorString(dev.vendorId), findDeviceString(dev.vendorId, dev.deviceId), findDeviceDesc(dev.vendorId, dev.deviceId),
                        findDeviceClass(dev.classCode), findDeviceSubclass(dev.classCode, dev.subclass)).c_str();
    char* data = (char*)malloc(str.length() + 1);
    memcpy(data, str.c_str(), str.length());
    data[str.length()] = 0;
    return stream::create(0x1000, str.length() + 1, _writeHndlr, _readHndlr, _closeHndlr, data);
}

int _mctlHndlr(void* tag, uint32_t id, void* in, void* out) {
    if (id == PCI_MCTL_GET_DEVICE_COUNT) {
        uint32_t* count = (uint32_t*)out;
        count[0] = fioDevices.size();
        return sizeof(uint32_t);
    }if (id == PCI_MCTL_GET_DEVICES) {
        PCIDeviceInfo_t* devs = (PCIDeviceInfo_t*)out;
        int count = fioDevices.size();
        for (int i = 0; i < count; i++) {
            devs[i] = fioDevices[i];
        }
        return sizeof(PCIDeviceInfo_t) * count;
    }
    else if (id == PCI_MCTL_GET_DEVICE) {
        PCIDeviceAddr_t* addr = (PCIDeviceAddr_t*)in;
        PCIDevice_t dev = getDevice(addr->bus, addr->func, addr->slot);
        memcpy(out, &dev, sizeof(PCIDevice_t));
        return sizeof(PCIDevice_t);
    }
    return -1;
}

extern "C"
bool _start() {
    kio::print(LOG_UNDEF "[pci] Creating /fio/dev/pci ...");
    fioDevices = vector<PCIDeviceInfo_t>();
    if (!fio::createDir("/dev/pci")) {
        kio::failed();
        kio::println("Could not create the pci folder!");
        return false;
    }
    kio::ok();

    kio::print(LOG_UNDEF "[pci] Enumerating devices...");
    for (int bus = 0; bus < 256; bus++) {
        for (int slot = 0; slot < 32; slot++) {
            for (int func = 0; func < 8; func++) {
                uint32_t vendor = readConfigWord(bus, slot, func, 0);
                if (vendor == 0xFFFF) {
                    continue;
                }
                PCIDevice_t device = getDevice(bus, slot, func);
                string name = string("/dev/pci/") + sprintf("%03u.%02u:%u", bus, slot, func);
                bool success = fio::mountStreamProvider(string(name).c_str(), FS_FLAG_O_R, _Provider, (void*)fioDevices.size());
                if (!success) {
                    kio::failed();
                    kio::printf("Could not register device file for %s!\n", name);
                    return false;
                }
                PCIDeviceInfo_t info;
                info.device = device;
                info.addr.bus = bus;
                info.addr.slot = slot;
                info.addr.func = func;
                fioDevices.push_back(info);
                
                if (device.headerType.isMultiFunc == 0) {
                    break;
                }
            }
        }
    }
    kio::ok();

    kio::print(LOG_UNDEF "[pci] Registering MCTL interface...");
    MCTLHandler_t handler;
    handler._handler = _mctlHndlr;
    handler.tag = NULL;
    if (!mctl::registerHndlr("/fio/dev/pci", handler)) {
        kio::failed();
        kio::println("Could not register MCTL handler for /fio/dev/pci!");
        return false;
    }
    kio::ok();

    return true;
}