#include "gpio.h"

#define RNG_CTRL        ((volatile unsigned int*)(MMIO_BASE+0x00104000))
#define RNG_STATUS      ((volatile unsigned int*)(MMIO_BASE+0x00104004))
#define RNG_DATA        ((volatile unsigned int*)(MMIO_BASE+0x00104008))
#define RNG_INT_MASK    ((volatile unsigned int*)(MMIO_BASE+0x00104010))

/**
 * Initialize the RNG
 */
void rand_init()
{
    *RNG_STATUS=0x40000;
    // mask interrupt
    *RNG_INT_MASK|=1;
    // enable
    *RNG_CTRL|=1;
}

/**
 * Return a random number between [min..max]
 */
unsigned int rand()
{
    // may need to wait for entropy: bits 24-31 store how many words are
    // available for reading; require at least one
    while(!((*RNG_STATUS)>>24)) asm volatile("nop");
    return *RNG_DATA;
}
