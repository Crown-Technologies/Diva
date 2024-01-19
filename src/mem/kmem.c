#include <stddef.h>
#include <ntypes.h>
#include <stdlib/mem.h>
#include <spinlock.h>
#include <diva/kernel.h>
#include <stdlib/time.h>
#include "mmu.h"

#include <driver/uart.h>

extern volatile u8 _end;

#define HEAP_START   (u64)&_end + 0x100000
#define OFFSET_END   0xffb000    /* 15 MB */
#define HEAP_END     (1024 * 1024 * 1024 - OFFSET_END)   /* 1024 MB */

#define MAX_ALLOC_ALLOWED 1024


struct kalloc_info {
    u64 addr;
    u64 size; 
} kalloc_info[MAX_ALLOC_ALLOWED];

struct spinlock kalloc_lk;


void *kmalloc(unsigned long long size) {

    acquire(&kalloc_lk);
    
    u64 addr = HEAP_START;

    for (u16 i = 0; i < MAX_ALLOC_ALLOWED; i++) {
        if (kalloc_info[i].addr != 0 &&
            (addr <= kalloc_info[i].addr < addr + size ||
            addr <= kalloc_info[i].addr + kalloc_info[i].size < addr + size)) {
                addr = kalloc_info[i].addr + kalloc_info[i].size;
            }
    }

    if (addr + size > HEAP_END) {
        release(&kalloc_lk);
        return NULL;
    }

    for (u16 i = 0; i < MAX_ALLOC_ALLOWED; i++) {
        if (kalloc_info[i].addr == 0) {
            kalloc_info[i].addr = addr;
            kalloc_info[i].size = size;

            release(&kalloc_lk);

            return (void*) addr;
        }
    }

    release(&kalloc_lk);

    return NULL;
}


void kfree(void *va) {
    if (va == NULL)
        return;

    acquire(&kalloc_lk);

    for (u16 i = 1; i < MAX_ALLOC_ALLOWED; i++)
        if (va == (void*) kalloc_info[i].addr)
            kalloc_info[i].addr = 0;

    release(&kalloc_lk);
}


void premmu_alloc_init() {
    lock_init(&kalloc_lk);
    kalloc_info[0].size = 0;
    kalloc_info[0].addr = HEAP_START;
    for (u16 i = 1; i < MAX_ALLOC_ALLOWED; i++)
        kalloc_info[i].addr = 0;
}

