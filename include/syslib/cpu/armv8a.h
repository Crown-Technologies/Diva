#ifndef ARMV8A_H
#define ARMV8A_H

#include <ntypes.h>

#define nop() asm volatile("nop")
#define sev() asm volatile("sev")
#define wfe() asm volatile("wfe")
#define isb() asm volatile("isb")
#define enable_irq() asm volatile("msr daifclr, #0x2" ::: "memory")
#define disable_irq() asm volatile("msr daifset, #0x2" ::: "memory")

static inline unsigned long long cpuid() {
    unsigned long long mpidr;
    asm volatile("mrs %0, mpidr_el1" : "=r"(mpidr));
    return mpidr & 0xff;
}

static inline u64 get_el() {
    u64 el;
    asm volatile ("mrs %0, CurrentEL" : "=r" (el));
    return (el>>2)&3;
}

#endif
