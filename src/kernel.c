#include <stdint.h>

#include "std/val.h"

#include "uart/uart.h"
#include "uart/mbox.h"
#include "uart/rand.h"
#include "uart/time.h"
#include "uart/system/system.h"
#include "uart/graphics/framebuffer.h"
#include "uart/graphics/console.h"
#include "uart/system/devices/blk.h"
#include "uart/system/devices/fs/fat.h"

#define NTESTS

#if defined(__cplusplus)
extern "C" // Use C linkage for kernel_main.
#endif


void kernel_main(uint64_t dtb_ptr32, uint64_t x1, uint64_t x2, uint64_t x3)
{
    ///// Start init
    uart_init();
    rand_init();

    FBInfo fb_info;
    fb_info.width = 1024;
    fb_info.height = 768;
    fb_info.depth = 32;
    fb_init(fb_info);
    ///// End init

    
    uart_printf("Board serial number: %16x\n", get_serial());

    //console_printf("Hello World!\n hello");
    //console_printf("Hello 多种语言 Многоязычный többnyelvű World!");
    
    #ifndef NTESTS
    uart_printf("[test] blk deivces\n")
        if (dev_init() == DEV_OK)
            if (fat_getpartition())
                fat_listdirectory();
            else uart_printf("FAT partition not found???\n");
        else uart_printf("Failed to init device\n");
    uart_printf("[test] random: %x\n", rand());
    #endif


    // echo everything back
    while(1) uart_send(uart_getc());
}

