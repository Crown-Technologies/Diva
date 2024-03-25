#include <syslib/proc.h>
#include <syslib/cpu.h>

struct cpu cpus[NCPU];

struct cpu *getcpu() {
    return &cpus[cpuid()];
}
