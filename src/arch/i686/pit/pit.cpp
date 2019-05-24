#include <pit/pit.h>
#include <misc/cpuio.h>

namespace pit {
    void command(uint8_t cmd) {
        outb(PIT_PORT_CHAN_MODE, cmd);
    }

    void setFrequency(uint8_t channel, uint32_t freq) {
        uint32_t divisor =  1193180 / freq;
        outb(channel, divisor & 0xFF);
        outb(channel, divisor >> 8);
    }

    uint16_t getCurrentCount(uint8_t channel) {
        outb(PIT_PORT_CHAN_MODE, 0);
        uint16_t l = inb(channel);
        uint16_t h = inb(channel);
        return (h << 8) | l;
    }
}