#include <driver/framebuffer.h>
#include <syslib/sh.h>
#include <syslib/kmem.h>
#include <stdlib/mem.h>
#include <stdlib/string.h>
#include <spinlock.h>
#include <ntypes.h>

struct spinlock sh_lk;

char* buffer;
u8 flag_clean = 1;

void sh_init() {
    lock_init(&sh_lk);

    buffer = kmalloc(1024 * 16); // 16 KB
    buffer[0] = '\0';
}

void sh_update() {
    sh_clear();
    fb_print(0, 0, buffer);
}

void sh_clear() {
    fb_flush();
    flag_clean = 1;
}

void sh_puts(char *s) {
    acquire(&sh_lk);
    u32 slen = strlen(s);
    u32 blen = 0;
    if (!flag_clean)
        blen = strlen(buffer);
    memcpy(buffer + blen, s, slen);
    buffer[blen + slen] = '\0';
    sh_update();
    flag_clean = 0;
    release(&sh_lk);
}


