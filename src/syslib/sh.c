#include <driver/framebuffer.h>


void console_printf(char *s) {
    fb_print(10, 10, s);
}


