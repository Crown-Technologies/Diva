#include <stdlib/mem.h>


void *memset(void *dst, int c, unsigned long long n) {
    char *d = dst;

    while(n-- > 0) *d++ = c;

    return dst;
}

void *bzero(void *dst, unsigned long long n) {
    memset(dst, 0, n);
    return dst;
}
