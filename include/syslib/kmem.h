#define PAGESIZE 4096

void alloc_init();

void *kalloc();
void *kmalloc(unsigned long long size);
void kfree(void *);
