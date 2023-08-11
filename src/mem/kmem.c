#include <stddef.h>
#include <ntypes.h>
#include <stdlib/mem.h>
#include <spinlock.h>
#include <diva/kernel.h>
#include <driver/uart.h>
#include "mmu.h"


extern volatile u8 _end;

#define HEAP_START   (u64)&_end
#define OFFSET_END   0xffb000    /* 15 MB */
#define HEAP_END     (1024*1024*16)//(1024 * 1024 * 1024 - OFFSET_END)   /* 1024 MB */

#define SECTIONSIZE  (2 * 1024 * 1024) /* 2MB */


struct header {
    struct header *next;
};

struct kallocator {
    struct spinlock lk;
    struct header *freelist;
} kallocator;


void *kalloc() {
    acquire(&kallocator.lk);

    struct header *new = kallocator.freelist;
    if (!new) {
        return NULL;
    }

    kallocator.freelist = new->next;

    release(&kallocator.lk);

    bzero((char*) new, PAGESIZE);
    
    return (void*) new;
}


void kfree(void *va) {
    if (va == NULL)
        return;
    if ((unsigned long long) va % PAGESIZE != 0)
        panic("bad va");

    bzero(va, PAGESIZE);
    
    struct header *p = (struct header *) va;

    acquire(&kallocator.lk);


    p->next = kallocator.freelist;
    kallocator.freelist = p;

    release(&kallocator.lk);
}


static inline u64 ksecend() {
    return (HEAP_START + SECTIONSIZE - 1) & ~(SECTIONSIZE - 1);
}


void premmu_alloc_init() {
    lock_init(&kallocator.lk);
    kallocator.freelist = NULL;

    for (u8 *p = HEAP_START; p + PAGESIZE <= (u8*) ksecend(); p += PAGESIZE)
        kfree(p);
}


void posmmu_alloc_init() {
    for (u8 *p = (u8*) ksecend(); p + PAGESIZE <= (u8*) HEAP_END; p += PAGESIZE)
        kfree(p);
}

