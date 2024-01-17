#include <diva/kernel.h>
#include <driver/uart.h>
#include <syslib/cpu/armv8a.h>


void panic(const char *s, ...) {
    disable_irq();

    uart_puts("\nkernel panic!\n");
    uart_puts(s);
    uart_puts("\n");
    while (1) { wfe(); }
}
