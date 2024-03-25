#include <diva/kernel.h>
#include <syslib/cpu.h>
#include <syslib/sh.h>

void panic(const char *s, ...) {
    disable_irq();

    sh_puts("\nkernel panic!\n");
    sh_puts(s);
    sh_puts("\n");
    halt_core();
}
