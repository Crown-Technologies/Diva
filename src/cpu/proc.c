#include <syslib/cpu/armv8a.h>
#include <syslib/proc.h>

struct cpu cpus[NCPU];

struct cpu *getcpu() {
    return &cpus[cpuid()];
}
