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


void kernel_secondary() {
    wait_ms(1000000);
    //sh_puts("Hello, core!");
    while (1) { wfe(); }
}

#if defined(__cplusplus)
extern "C" // Use C linkage for kernel_main.
#endif

void kernel_main(uint64_t core_id)
{
    if (core_id != 0) {
        kernel_secondary();
    }
    ///// Start init
    uart_init();
    premmu_alloc_init();
    mmu_init();

    u32 board_serial = get_serial();

    fb_init(800, 600, 32);

    int y = 0;
    if (board_serial == 0) // QEMU
        y = 27;

    sh_init(y); // Enables framebuffer shell

    u8 el = 0;
    sh_puts("[info] current EL is: ");
    sh_puts((el = get_el()) == 1 ? "1 (kernel mode)" : htoa(el));
    sh_puts("\n");

    sh_puts("[init] rand...");
    rand_init(); sh_puts("[OK]\n");
    ///// End init

    sh_puts("[info] board serial number: ");
    sh_puts(strldz(htoa(board_serial << 16), 8));
    sh_puts(strldz(htoa(board_serial), 8));
    sh_puts("\n");

    sh_puts("[test] blk devices...");
    u8 *mbr = kmalloc(512);
    u64 lba;
    if (dev_init() == DEV_OK)
        if((lba = fat_get_partition(mbr))) {
            sh_puts("[OK]");
            sh_puts(fat_get_type(mbr) > 0 ? "FAT32\n" : "FAT16\n");
        }
        else sh_puts("[ERR] FAT partition not found\n");
    else sh_puts("[ERR] Failed to init device\n");

    #ifndef NTESTS
    sh_puts("[info] NTESTS removed. Tests enabled\n");

    sh_puts("[test] random num: 0x");
    sh_puts(strldz(htoa(rand()), 8));
    sh_puts("\n");
    
    sh_puts("[info] tests end\n");
    #endif

    while (1) { wfe(); }
}

