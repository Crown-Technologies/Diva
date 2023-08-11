#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <stdbool.h>

struct spinlock {
  unsigned char locked;
  int cpuid;
};

bool holding(struct spinlock *lk);
void acquire(struct spinlock *lk);
void release(struct spinlock *lk);
void lock_init(struct spinlock *lk);

#endif
