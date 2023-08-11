#include <stdint.h>
#include <stddef.h>
#include <ntypes.h>

#include <diva/kernel.h>
#include <driver/uart.h>
#include <driver/blk.h>
#include <driver/fs/fat.h>

#include <syslib/cpu/armv8a.h>
#include <syslib/system.h>
#include <syslib/kmem.h>
#include <stdlib/rand.h>

#include "mem/mmu.h"

//#define NTESTS

#if defined(__cplusplus)
extern "C" // Use C linkage for kernel_main.
#endif


void kernel_main(uint64_t dtb_ptr32, uint64_t x1, uint64_t x2, uint64_t x3)
{
    unsigned long el;
    asm volatile ("mrs %0, CurrentEL" : "=r" (el));
    uart_puts("[info] current EL is: ");
    uart_hex((el>>2)&3);
    uart_puts("\n");
    ///// Start init
    uart_puts("[init] UART0...");
    uart_init(); uart_puts("[OK]\n");
    uart_puts("[init] pre-MMU alloc...");
    premmu_alloc_init(); uart_puts("[OK]\n");
    uart_puts("[init] MMU...");
    mmu_init(); uart_puts("[OK]\n");
    uart_puts("[init] post-MMU alloc...");
    posmmu_alloc_init(); uart_puts("[OK]\n");
    uart_puts("[init] rand...");
    rand_init(); uart_puts("[OK]\n");
    //FBInfo fb_info;
    //fb_info.width = 1024;
    //fb_info.height = 768;
    //fb_info.depth = 32;
    //fb_init(fb_info);
    ///// End init

    
    uart_puts("[info] board serial number: ");
    u32 board_serial = get_serial();
    uart_hex(board_serial<<16);
    uart_hex(board_serial);
    uart_puts("\n");
    
    #ifndef NTESTS
    uart_puts("[info] NTESTS disabled. Tests enabled\n");
    uart_puts("[test] writing through MMIO mapped in higher half: "); 
    char *s="Hello, world!\r\n";
    #define KERNEL_UART0_DR        ((volatile unsigned int*)0xFFFFFFFFFFE00000)
    #define KERNEL_UART0_FR        ((volatile unsigned int*)0xFFFFFFFFFFE00018)
      
    while(*s) {
        /* wait until we can send */
        do{asm volatile("nop");}while(*KERNEL_UART0_FR&0x20);
        /* write the character to the buffer */
        *KERNEL_UART0_DR=*s++;
    }

    uart_puts("[test] random num: 0x");
    uart_hex(rand());
    uart_puts("\n");


    //uart_printf("[test] blk deivces\n");
    //    if (dev_init() == DEV_OK)
    //        if (fat_getpartition())
    //            uart_printf("[OK]\n");
    //        else uart_printf("FAT partition not found???\n");
    //    else uart_printf("Failed to init device\n");
    //uart_printf("[test] random: %x\n", rand());
    #endif

    while (1) { wfe(); }
}

