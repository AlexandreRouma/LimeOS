#include <new.h>
#include <misc/liballoc.h>

void *operator new(long unsigned int size)
{
    return malloc(size);
}
 
void *operator new[](long unsigned int size)
{
    return malloc(size);
}
 
void operator delete(void *p)
{
    free(p);
}

void operator delete(void *p, unsigned long size) {
    free(p);
}
 
void operator delete[](void *p)
{
    free(p);
}

void operator delete[](void *p, unsigned long size)
{
    free(p);
}


