#include <spinlock.h>
#include <syslib/proc.h>
#include <syslib/cpu.h>

void pushcli();
void popcli();


bool holding(struct spinlock *lk) {
    return lk->locked && lk->cpuid == cpuid();
}

void acquire(struct spinlock *lk) {
    pushcli();

    if (holding(lk))
        return;

    //while (__sync_lock_test_and_set(&lk->locked, 1) != 0);

    isb();

    lk->cpuid = cpuid();
}

void release(struct spinlock *lk) {
    if (!holding(lk))
        return;

    lk->cpuid = -1;

    isb();

    asm volatile("str wzr, %0" : "=m" (lk->locked));

    popcli();
}

void pushcli() {
    struct cpu *cpu = getcpu();

    if (cpu->cli_depth == 0 && cpu->intr_enabled)
        enable_irq();
}

void popcli() {
    struct cpu *cpu = getcpu();

    if (cpu->cli_depth == 0)
        return;

    if (--cpu->cli_depth == 0 && cpu->intr_enabled)
        enable_irq();
}

void lock_init(struct spinlock *lk) {
    lk->locked = 0;
    lk->cpuid = -1;
}

