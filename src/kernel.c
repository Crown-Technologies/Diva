#include <stdint.h>

#include "std/val.h"

#include "uart/uart.h"
#include "uart/mbox.h"
#include "uart/rand.h"
#include "uart/time.h"
#include "uart/system/system.h"
#include "uart/graphics/framebuffer.h"
#include "uart/graphics/console.h"


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

    
    uart_printf("SoC serial number: %16x\n", get_serial());

    //console_printf("Hello World!\n hello");
    //console_printf("Hello 多种语言 Многоязычный többnyelvű World!");

    #ifndef NDEBUG
    uart_printf("[test] random: %x\n", rand());

    uart_printf("[test] waiting 1000000 ms: ");
    wait_ms(1000000);
    uart_printf("OK\n");
    #endif


    // echo everything back
    while(1) {
        uart_send(uart_getc());
    }
}

