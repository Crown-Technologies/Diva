#include <stdlib/mem.h>

void *memcpy(void *dst, void *src, u64 n) {
    u8 *d = dst;
    u8 *s = src;
    while (n-- > 0)
        *d++ = *s++;
    return dst;
}

void *memset(void *dst, u8 c, u64 n) {
    u8 *d = dst;
    while (n-- > 0)
        *d++ = c;
    return dst;
}

void *bzero(void *dst, u64 n) {
    memset(dst, 0, n);
    return dst;
}
