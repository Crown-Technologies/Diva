#include "framebuffer.h"
#include "../../std/sprintf.h"

extern volatile unsigned char _end;


void console_printf(char *fmt, ...) {
    __builtin_va_list args;
    __builtin_va_start(args, fmt);
    // we don't have memory allocation yet, so we
    // simply place our string after our code
    char *s = (char*)&_end;
    // use sprintf to format our string
    vsprintf(s,fmt,args);
    // print out as usual
    fb_print(10, 10, s);
    
}


