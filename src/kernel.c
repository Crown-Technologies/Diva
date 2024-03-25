#include <ntypes.h>
#include <stddef.h>
#include <stdint.h>

#include <diva/kernel.h>
#include <driver/blk.h>
#include <driver/framebuffer.h>
#include <driver/fs/fat.h>
#include <driver/uart.h>

#include <spinlock.h>
#include <syslib/cpu.h>
#include <syslib/kmem.h>
#include <syslib/sh.h>
#include <syslib/system.h>

#include <stdlib/mem.h>
#include <stdlib/rand.h>
#include <stdlib/string.h>
#include <stdlib/time.h>

#include <driver/gpio.h>
#include <syslib/mmu.h>

// #define NTESTS

struct spinlock kmain_lk;

u8 cores_num = 0;

void kernel_secondary() {
    // wait_ms(1000000);
    // sh_puts("Hello, core!");
    // void *a = kmalloc(8);
    // memset(a, 'A', 8);
    halt_core();
}

#if defined(__cplusplus)
extern "C" // Use C linkage for kernel_main.
#endif

    void
    kernel_main(u64 core_id) {
    cores_num++;
    if (core_id) {
        acquire(&kmain_lk);
        kernel_secondary();
    } else {
        lock_init(&kmain_lk);
        uart_init();
        alloc_init();

        fb_init(800, 600, 32);
        sh_init(); // Enables framebuffer shell

        if (mmu_init() == 1)
            panic("Cannot map memory: 4k granule or 36 address space not "
                  "supporded");
        release(&kmain_lk);
    }

    sh_puts("[SYSTEM INFO]\n");

    sh_puts("CPU_ARCH: ");
    sh_puts(CPU_ARCH);
    sh_puts("\n");

    sh_puts("cores_num: ");
    sh_puts(itoa(cores_num));
    sh_puts("\n");

    u8 el = 0;
    sh_puts("current EL is: ");
    sh_puts((el = get_el()) == 1 ? "1 (kernel mode)" : htoa(el));
    sh_puts("\n");

    u32 board_serial = get_serial();
    sh_puts("board serial number: ");
    sh_puts(strldz(htoa(board_serial << 16), 8));
    sh_puts(strldz(htoa(board_serial), 8));
    sh_puts("\n\n");

    sh_puts("[INIT]\n");
    sh_puts("rand...");
    rand_init();
    sh_puts("[OK]\n\n");
    ///// End init

    /*sh_puts("[test] blk devices...");
    u8 *mbr = kmalloc(512);
    u64 lba;
    if (dev_init() == DEV_OK)
        if((lba = fat_get_partition(mbr))) {
            sh_puts("[OK]");
            sh_puts(fat_get_type(mbr) > 0 ? "FAT32\n" : "FAT16\n");
        }
        else sh_puts("[ERR] FAT partition not found\n");
    else sh_puts("[ERR] Failed to init device\n");*/

#ifndef NTESTS
    sh_puts("[TESTS]\n");

    sh_puts("random nums:\n");
    for (u8 i = 0; i < 20; i++) {
        sh_puts("0x");
        sh_puts(strldz(htoa(rand()), 8));
        sh_puts("\n");
    }

    sh_puts("[info] tests ended\n");
#endif
}
