#include <stdint.h>

void syscall(uint32_t id, uint32_t ebx, uint32_t ecx, uint32_t edx);

void KERN_EXEC_SYSCALL(uint32_t id, uint32_t ebx, uint32_t ecx, uint32_t edx);