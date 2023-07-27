#include <stdint.h>

#include "stdval.h"

#include "uart/uart.h"
#include "uart/mbox.h"
#include "uart/rand.h"
#include "uart/time.h"
#include "uart/system/system.h"


#if defined(__cplusplus)
extern "C" // Use C linkage for kernel_main.
#endif


void kernel_main(uint64_t dtb_ptr32, uint64_t x1, uint64_t x2, uint64_t x3)
{
    uart_init();
    rand_init();

    
    uart_puts("SoC serial number: ");
    unsigned long serial = get_serial();
    uart_hex(serial >> 36);
    uart_hex(serial);
    uart_puts("\n");

    #ifndef NDEBUG
    uart_puts("[test] random: ");
    uart_hex(rand());
    uart_puts("\n");


    uart_puts("[test] waiting 1000000 microsec: ");
    wait_ms(1000000);
    uart_puts("OK\n");
    #endif


    // echo everything back
    while(1) {
        uart_send(uart_getc());
    }
}

