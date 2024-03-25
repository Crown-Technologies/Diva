#ifndef STDLIB
#define STDLIB

#include <ntypes.h>
#include <stddef.h>

void *malloc(u64 size);

void *memcpy(void *dst, void *src, u64 n);
void *memset(void *dst, u8 c, u64 n);
void *bzero(void *dst, u64 n);

#endif
