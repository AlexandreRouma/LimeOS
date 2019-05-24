#pragma once
#include <stdint.h>

#define PIT_PORT_CHAN_0         0x40
#define PIT_PORT_CHAN_1         0x41
#define PIT_PORT_CHAN_2         0x42
#define PIT_PORT_CHAN_MODE      0x43

#define PIT_BCD                 1

#define PIT_MODE_TERM_COUNT     0 << 1
#define PIT_MODE_HW_RETRIG_SS   1 << 1
#define PIT_MODE_RATE_GEN       2 << 1
#define PIT_MODE_SQUARE_GEN     3 << 1
#define PIT_MODE_SW_TRIG_STROBE 4 << 1
#define PIT_MODE_HW_TRIG_STROBE 5 << 1

#define PIT_ACCESS_LATCH        0 << 4
#define PIT_ACCESS_LOW_BYTE     1 << 4
#define PIT_ACCESS_HIGH_BYTE    2 << 4
#define PIT_ACCESS_BOTH         3 << 4

#define PIT_CHAN_0              0 << 6
#define PIT_CHAN_1              1 << 6
#define PIT_CHAN_2              2 << 6
#define PIT_CHAN_READBACK       3 << 6

namespace pit {
    void command(uint8_t cmd);
    void setFrequency(uint8_t channel, uint32_t freq);
    uint16_t getCurrentCount(uint8_t channel);
}
