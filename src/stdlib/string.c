#include <ntypes.h>
#include <stdlib/string.h>
#include <stdlib/mem.h>


unsigned long long strlen(void *str) {
    char* s = str;
    unsigned long long n = 0;
    while (s[n]) n++;
    return n;
}

char* ntoa(int a, unsigned char base) {
  // Max text needs occur with itostr(dst, size, INT_MIN, 2)
  char buffer[sizeof a * 8 + 1 + 1]; 
  static const char digits[36] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    if (base < 2 || base > 36)
        return NULL;
    // Start filling from the end
    char* p = &buffer[sizeof buffer - 1];
    *p = '\0';

    // Work with negative `int`   
    int an = a < 0 ? a : -a;

    do {
        *(--p) = digits[-(an % base)];
    an /= base;
    } while (an);

    if (base == 10 && a < 0) {
        *(--p) = '-';
    }

    u64 size_used = &buffer[sizeof(buffer)] - p;
    char* dst = kmalloc(size_used);
    return memcpy(dst, p, size_used);
}


char* itoa(int a) {
    return ntoa(a, 10);
}

char* htoa(int a) {
    return ntoa(a, 16);
}

char* btoa(int a) {
    return ntoa(a, 2);
}
