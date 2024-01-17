#ifndef ARMV8A_H
#define ARMV8A_H

#include <ntypes.h>

static inline unsigned long long cpuid() {
    unsigned long long mpidr;
    asm volatile("mrs %0, mpidr_el1" : "=r"(mpidr));
    return mpidr & 0xff;
}

static inline void wfe() {
    asm volatile("wfe");
}

static inline void isb() {
    asm volatile("isb");
}

static inline void enable_irq() {
    asm volatile("msr daifclr, #0x2" ::: "memory");
}

static inline void disable_irq() {
    asm volatile("msr daifset, #0x2" ::: "memory");
}

static inline u64 get_el() {
    u64 el;
    asm volatile ("mrs %0, CurrentEL" : "=r" (el));
    return (el>>2)&3;
}

#endif
