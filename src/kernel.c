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
    u8 el = 0;

    kprint("[info] current EL is: ");
    kprintln((el = get_el()) == 1 ? "1 (kernel mode)" : htoa(el));

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
    kprint(strldz(htoa(board_serial << 16), 8));
    kprintln(strldz(htoa(board_serial), 8));

    
    #ifndef NTESTS
    kprintln("[info] NTESTS removed. Tests enabled");

    kprint("[test] random num: 0x");
    kprintln(strldz(htoa(rand()), 8));


    kprint("[test] blk deivces...");
    u8 *mbr = kmalloc(512);
    u64 lba;
    if (dev_init() == DEV_OK)
        if((lba = fat_get_partition(mbr))) {
            kprint("[OK]");
            kprintln(fat_get_type(mbr) > 0 ? "FAT32" : "FAT16");
        }
        else kprintln("[ERR] FAT partition not found");
    else kprintln("[ERR] Failed to init device");
    
    kprintln("[info] tests end");
    #endif

    while (1) { wfe(); }
}

