
#define PAGESIZE 0x1000

// granularity
#define PT_PAGE     0b11        // 4k granule
#define PT_BLOCK    0b01        // 2M granule
// accessibility
#define PT_KERNEL   0           // privileged, supervisor EL1 access only DONOTUSE
#define PT_USER     (1<<6)      // unprivileged, EL0 access allowed
#define PT_RW       0           // read-write DONOTUSE
#define PT_RO       (1<<7)      // read-only
#define PT_AF       (1<<10)     // accessed flag
#define PT_NX       (1UL<<54)   // no execute
// shareability
#define PT_OSH      (2<<8)      // outter shareable
#define PT_ISH      (3<<8)      // inner shareable
// defined in MAIR register
#define PT_MEM      0           // normal memory DONOTUSE
#define PT_DEV      (1<<2)      // device MMIO

#define TTBR_CNP    1


.global mmu_initt
mmu_initt:
    // Args:
    // x0 – mmio_base

    // Data:
    // x1 – data_page
    // x2 – _end

    // Setup x1
    ldr x1, =_data
    mov x2, PAGESIZE
    udiv x1, x1, x2

    // Setup x2
    ldr x2, =_end

    // TTBR0, identity L1
    add x3, x2, PAGESIZE * 2
    orr x3, x3, PT_PAGE
    orr x3, x3, PT_AF
    orr x3, x3, PT_USER
    orr x3, x3, PT_ISH
    str x3, [x2]

    // identity L2, first 2M block
    add x3, x2, PAGESIZE * 3
    orr x3, x3, PT_PAGE
    orr x3, x3, PT_AF
    orr x3, x3, PT_USER
    orr x3, x3, PT_ISH
    add x4, x2, 0x400
    str x3, [x4]


    // identity L2 2M blocks
    lsr x5, x0, 21 // b
    mov x6, 1 // r
0:
    lsl x3, x6, 21
    orr x3, x3, PT_BLOCK
    orr x3, x3, PT_AF
    orr x3, x3, PT_NX
    orr x3, x3, PT_USER

    // Different attributes if device memory
    cmp x6, x5
    b.hs 1f

    orr x3, x3, PT_ISH
    b 2f
1:
    mov x7, PT_OSH
    orr x7, x7, PT_DEV
    orr x3, x3, x7
2:
    add x4, x2, 0x400
    add x4, x4, x6
    str x3, [x4]

    add x6, x6, 1
    cmp x6, 0x200
    b.lo 0b

    // identity L3
    mov x5, 0 // r
0:
    mov x6, PAGESIZE
    mul x3, x5, x6
    orr x3, x3, PT_PAGE
    orr x3, x3, PT_AF
    orr x3, x3, PT_USER
    orr x3, x3, PT_ISH

    cmp x5, 0x80
    b.lo 1f
    cmp x5, x1 
    b.hs 1f

    orr x3, x3, PT_RO
    b 2f
1:
    mov x6, PT_RW
    orr x6, x6, PT_NX
    orr x3, x3, x6
2:
    add x4, x2, 0x600
    add x4, x4, x5
    str x3, [x4]

    add x6, x6, 1
    cmp x6, 0x200
    b.lo 0b

    // TTBR1, kernel L1
    add x3, x2, PAGESIZE * 4
    orr x3, x3, PT_PAGE
    orr x3, x3, PT_AF
    orr x3, x3, PT_ISH
    add x4, x2, 0x3ff
    str x3, [x4]

    // kernel L2
    add x3, x2, PAGESIZE * 5
    orr x3, x3, PT_PAGE
    orr x3, x3, PT_AF
    orr x3, x3, PT_ISH
    add x4, x2, 0x9ff
    str x3, [x4]

    // kernel L3
    add x0, x0, 0x00201000
    orr x0, x0, PT_PAGE
    orr x0, x0, PT_AF
    orr x0, x0, PT_NX
    orr x0, x0, PT_OSH
    orr x0, x0, PT_DEV
    add x3, x2, 0xa00
    str x0, [x3]

    /* check for 4k granule and at least 36 bits physical address bus */
    mrs x3, id_aa64mmfr0_el1
    and x0, x3, 0xf // b

    mov x4, 0xf
    lsl x4, x4, 28
    and x4, x4, x3
    mvn x3, x0
    orr x3, x3, x4

    cbnz x3, 0f // ERROR: 4k granule or 36 bit address space not supported
    b 1f

0:
    mov x0, 1
    b error

1:

    // set Memory Attributes array, indexed by PT_MEM, PT_DEV, PT_NC
    mov x3, 0xff
    orr x3, x3, (0x4 << 8)
    mov x5, 0x44
    lsl x4, x5, 16
    orr x3, x3, x4
    msr mair_el1, x3

    // specify mapping characteristics in translate control register
    lsl x0, x0, 32
    orr x0, x0, (0b10 << 30)
    orr x0, x0, (0b11 << 28)
    orr x0, x0, (0b01 << 26)
    orr x0, x0, (0x01 << 24)
    orr x0, x0, (0b11 << 12)
    orr x0, x0, (0b01 << 10)
    orr x0, x0, (0b01 <<  8)
    mov x1, 25
    orr x0, x0, x1
    lsl x1, x1, 16
    orr x0, x0, x1
    msr tcr_el1, x0
    isb

    // tell the MMU where our translation tables are. TTBR_CNP bit not documented, but required
    add x0, x2, TTBR_CNP
    msr ttbr0_el1, x0 // lower half, user space
    add x0, x0, PAGESIZE
    msr ttbr1_el1, x0 // upper half, kernel space

    dsb ish 
    isb

    // enable page translation
    mrs x0, sctlr_el1
    ldr x1, =0xc00800
    orr x0, x0, x1
    mov x1,     (1 << 25) // clear EE, little endian translation tables
    orr x1, x1, (1 << 24)
    orr x1, x1, (1 << 19)
    orr x1, x1, (1 << 12)
    orr x1, x1, (1 << 4)
    orr x1, x1, (1 << 3)
    orr x1, x1, (1 << 2)
    orr x1, x1, (1 << 1)
    mvn x1, x1
    and x0, x0, x1
    orr x0, x0, 1

    msr sctlr_el1, x0 
    isb

    mov x0, 0
    ret


error:
    ret
