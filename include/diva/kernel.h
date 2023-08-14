#ifndef KERNEL_H
#define KERNEL_H

void panic(const char* s, ...);
void kprintln(const char* s);
void kprint(const char* s);

void enable_sh();

#endif
