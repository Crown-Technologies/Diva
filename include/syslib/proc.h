#ifndef PROC_H
#define PROC_H


#define NCPU 64

struct cpu {
    int cli_depth;
    int intr_enabled;
};

struct cpu *getcpu(void);

#endif
