#include <stdint.h>
#include <stddef.h>
#include <ntypes.h>

#include <diva/kernel.h>
#include <driver/uart.h>
#include <driver/blk.h>
#include <driver/fs/fat.h>
#include <driver/framebuffer.h>

#include <syslib/cpu/armv8a.h>
#include <syslib/system.h>
#include <syslib/kmem.h>
#include <syslib/sh.h>

#include <stdlib/rand.h>
#include <stdlib/time.h>
#include <stdlib/string.h>
#include <stdlib/mem.h>

#include "mem/mmu.h"

//#define NTESTS

#if defined(__cplusplus)
extern "C" // Use C linkage for kernel_main.
#endif


void kernel_main(uint64_t dtb_ptr32, uint64_t x1, uint64_t x2, uint64_t x3)
{
    kprint("[info] current EL is: ");
    kprintln(htoa(get_el()));

    ///// Start init
    kprint("[init] UART0...");
    uart_init(); kprintln("[OK]");

    kprint("[init] pre-MMU alloc...");
    premmu_alloc_init(); kprintln("[OK]");

    kprint("[init] MMU...");
    mmu_init(); kprintln("[OK]");

    enable_sh(); // Enables framebuffer shell

    kprint("[init] rand...");
    rand_init(); kprintln("[OK]");
    ///// End init

    kprint("[info] board serial number: ");
    u32 board_serial = get_serial();
    kprint(htoa(board_serial<<16));
    kprintln(htoa(board_serial));

    
    #ifndef NTESTS
    kprintln("[info] NTESTS disabled. Tests enabled");

    kprint("[test] random num: 0x");
    kprintln(htoa(rand()));


    //uart_puts("[test] blk deivces...");
    //    if (dev_init() == DEV_OK)
    //        if (fat_getpartition())
    //            uart_puts("[OK]\n");
    //        else uart_puts("FAT partition not found???\n");
    //    else uart_puts("Failed to init device\n");
    #endif

    while (1) { wfe(); }
}

