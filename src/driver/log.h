#ifndef DRI_LOG_H
#define DRI_LOG_H

#include <driver/uart.h>

#define BLKDEBUG

void blklog(char* s) {
    #ifdef BLKDEBUG
    uart_puts(s);
    #endif
}

void blklogh(unsigned long h) {
    #ifdef BLKDEBUG
    uart_hex(h);
    #endif
}

#endif
