#include <diva/kernel.h>
#include <syslib/sh.h>
#include <driver/framebuffer.h>
#include <driver/uart.h>
#include <ntypes.h>


u8 sh_enabled = 0;

void enable_sh() {
    FBInfo fb_info;
    fb_info.width = 1024;
    fb_info.height = 768;
    fb_info.depth = 32;
    fb_init(fb_info);
    sh_init();

    sh_enabled = 1;
}


void kprint(const char* s) {
    if (sh_enabled) {
        sh_puts(s);
    } else {
        uart_puts(s);
    }
}

void kprintln(const char* s) {
    kprint(s);
    kprint("\n");
}
