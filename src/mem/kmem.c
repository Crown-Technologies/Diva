#include <stddef.h>
#include <ntypes.h>
#include <spinlock.h>

extern volatile u8 _end;

#define HEAP_START ((u64)&_end + 8)
#define HEAP_END   (1024 * 1024 * 1024 - 16) /* 1024 MB */

struct kalloc_info {
    u64 addr;
    u64 size; 
} *kalloc_info = (struct kalloc_info*) HEAP_END - sizeof(struct kalloc_info);

u64 bottom_border_heap_info = HEAP_END - sizeof(struct kalloc_info);

struct spinlock kalloc_lk;


void *kmalloc(unsigned long long size) {

    acquire(&kalloc_lk);
    
    u64 addr = HEAP_START;

    for (struct kalloc_info* ptr = (struct kalloc_info*) HEAP_END; (u64) ptr > bottom_border_heap_info; ptr--) {
        if (ptr->addr != 0 &&
            (addr <= ptr->addr < addr + size ||
            addr <= ptr->addr + ptr->size < addr + size)) {
                addr = ptr->addr + ptr->size;
            }
    }

    if (addr + size > bottom_border_heap_info) {
        release(&kalloc_lk);
        return NULL;
    }

    for (struct kalloc_info* ptr = (struct kalloc_info*) HEAP_END; (u64) ptr > bottom_border_heap_info; ptr--) {
        if (ptr->addr == 0) {
            ptr->addr = addr;
            ptr->size = size;
            bottom_border_heap_info--;
            ptr[-1].addr = 0;

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

    for (struct kalloc_info* ptr = (struct kalloc_info*) HEAP_END; (u64) ptr > bottom_border_heap_info; ptr--)
        if (va == (void*) ptr->addr) {
            ptr->addr = 0;
            bottom_border_heap_info++;
        }

    release(&kalloc_lk);
}


void premmu_alloc_init() {
    lock_init(&kalloc_lk);
    kalloc_info[0].size = 0;
    kalloc_info[0].addr = HEAP_START;
}

