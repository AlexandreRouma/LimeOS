#include <new.h>
#include <kapi.h>

void *operator new(long unsigned int size)
{
    return kapi::api.mm.malloc(size);
}
 
void *operator new[](long unsigned int size)
{
    return kapi::api.mm.malloc(size);
}
 
void operator delete(void *p)
{
    kapi::api.mm.free(p);
}

void operator delete(void *p, unsigned long size) {
    kapi::api.mm.free(p);
}
 
void operator delete[](void *p)
{
    kapi::api.mm.free(p);
}

void operator delete[](void *p, unsigned long size)
{
    kapi::api.mm.free(p);
}


