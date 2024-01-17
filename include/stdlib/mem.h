#ifndef STDLIB
#define STDLIB

#include <stddef.h>

void *malloc(unsigned long long size);

void *memcpy(void *dst, void* src, unsigned long long n);
void *memset(void *dst, int c, unsigned long long n);
void *bzero(void *dst, unsigned long long n);

#endif
