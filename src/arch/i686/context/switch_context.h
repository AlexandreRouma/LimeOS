#pragma once

extern "C" {
    extern void _ASM_SWITCH_TASK_ISR(void);
}

void addTask(uint32_t ptr, uint32_t stack);