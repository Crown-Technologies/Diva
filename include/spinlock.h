#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <ntypes.h>

struct spinlock {
  u8 locked;
  i8 cpuid;
};

bool holding(struct spinlock *lk);
void acquire(struct spinlock *lk);
void release(struct spinlock *lk);
void lock_init(struct spinlock *lk);

#endif
