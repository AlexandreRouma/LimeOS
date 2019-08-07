#pragma once
#include <stdint.h>

#define FB_MCTL_CMD_GETINFO     1

struct FramebufferInfo_t {
    uint32_t addr;
    uint32_t width;
    uint32_t height;
    uint16_t bpp;
    uint32_t pitch;
};