#ifndef PROC_H
#define PROC_H

#include <ntypes.h>

#define NCPU 64

struct cpu {
    u8 cli_depth;
    u8 intr_enabled;
};

struct cpu *getcpu(void);

#endif
