#include <misc/liballoc.h>

void *operator new(long unsigned int size);
void *operator new[](long unsigned int size);
void operator delete(void *p);
void operator delete(void *p, unsigned long size);
void operator delete[](void *p);
void operator delete[](void *p, unsigned long size);