#ifndef CPU_H
#define CPU_H

#include <ntypes.h>

    //#if defined(__aarch64__) || defined(_M_ARM64)

    #define CPU_ARCH "aarch64"

    #define nop() asm volatile("nop")
    #define sev() asm volatile("sev")
    #define wfe() asm volatile("wfe")
    #define wfi() asm volatile("wfi")
    #define isb() asm volatile("isb")
    #define enable_irq() asm volatile("msr daifclr, #0x2" ::: "memory")
    #define disable_irq() asm volatile("msr daifset, #0x2" ::: "memory")

    //#endif

    extern u64 cpuid();
    extern u64 get_el();
    extern void halt_core();


#endif
