#include <stddef.h>
#include <stdint.h>

static uint32_t MMIO_BASE;
static int RPI;

// Setting up the MMIO base, depends on board type.
static inline void mmio_init(uint32_t reg)
{
    switch ((reg >> 4) & 0xFFF) {
        case 0xD03: RPI = 3; MMIO_BASE = 0x3F000000; break;
        case 0xD08: RPI = 4; MMIO_BASE = 0xFE000000; break;
        default:    RPI =-1; MMIO_BASE = 0x20000000; break;
    }
}

// MMIO
static inline void mmio_write(uint32_t reg, uint32_t data)
{
    *(volatile uint32_t*)(MMIO_BASE + reg) = data;
}

static inline uint32_t mmio_read(uint32_t reg)
{
    return *(volatile uint32_t*)(MMIO_BASE + reg);
}

// Loop <delay> times in a way that the compiler won't optimize away.
static inline void delay(int32_t count)
{
    asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
            : "=r" (count): [count]"0"(count) : "cc");
}


enum
{
    // The offsets for reach register.
    GPIO_BASE = 0x200000,

    // Controls actuation of pull up/down to ALL GPIO pins.
    GPPUD = (GPIO_BASE + 0x98),

    // Controls actuation of pull up/down for specific GPIO pin.
    GPPUDCLK0 = (GPIO_BASE + 0x98),

    // The base address for UART
    UART0_BASE = (GPIO_BASE + 0x1000), // rpi4 ~ 0xFE201000, raspi2 & 3 ~ 0x3F201000, rpi1 ~ 0x20201000

    // The offsets for reach registers for the UART.
    UART0_DR     = (UART0_BASE + 0x00),
    UART0_RSRECR = (UART0_BASE + 0x04),
    UART0_FR     = (UART0_BASE + 0x18),
    UART0_ILPR   = (UART0_BASE + 0x20),
    UART0_IBRD   = (UART0_BASE + 0x24),
    UART0_FBRD   = (UART0_BASE + 0x28),
    UART0_LCRH   = (UART0_BASE + 0x2C),
    UART0_CR     = (UART0_BASE + 0x30),
    UART0_IFLS   = (UART0_BASE + 0x34),
    UART0_IMSC   = (UART0_BASE + 0x38),
    UART0_RIS    = (UART0_BASE + 0x3C),
    UART0_MIS    = (UART0_BASE + 0x40),
    UART0_ICR    = (UART0_BASE + 0x44),
    UART0_DMACR  = (UART0_BASE + 0x48),
    UART0_ITCR   = (UART0_BASE + 0x80),
    UART0_ITIP   = (UART0_BASE + 0x84),
    UART0_ITOP   = (UART0_BASE + 0x88),
    UART0_TDR    = (UART0_BASE + 0x8C),

    // The offsets for Mailbox registers
    MBOX_BASE    = 0xB880,
    MBOX_READ    = (MBOX_BASE + 0x00),
    MBOX_STATUS  = (MBOX_BASE + 0x18),
    MBOX_WRITE   = (MBOX_BASE + 0x20)
};

// A Mailbox message with set clock rate of PL011 to 3MHz tag
volatile unsigned int __attribute__((aligned(16))) mbox[9] = {
    9*4, 0, 0x38002, 12, 8, 2, 3000000, 0, 0
};

void uart_init()
{
    uint32_t reg;

    asm volatile ("mrs %x0, midr_el1" : "=r" (reg));

    mmio_init(reg);

    // Disable UART0.
    mmio_write(UART0_CR, 0x00000000);
    // Setup the GPIO pin 14 && 15

    // Disable pull up/down for all GPIO pins & delay for 150 cycles.
	mmio_write(GPPUD, 0x00000000);
	delay(150);

    // Disable pull up/down for pin 14,15 & delay for 150 cycles.
    mmio_write(GPPUDCLK0, (1 << 14) | (1 << 15));
    delay(150);

    // Write 0 to GPPUDCLK0 to make it take effect.
    mmio_write(GPPUDCLK0, 0x00000000);

    // Clear pending interrupts.
    mmio_write(UART0_ICR, 0x7FF);

    if (RPI >= 3) {
        // UART_CLOCK = 30000000;
        unsigned int r = (((unsigned int)(&mbox) & ~0xF) | 8);
        // wait until we can talk to the VC
        while (mmio_read(MBOX_STATUS) & 0x80000000) {}
        // sent the message to property channel and wait for the response
        mmio_write(MBOX_WRITE, r);
        while ((mmio_read(MBOX_STATUS) & 0x40000000) || mmio_read(MBOX_READ) != r) {}
    }

    // Divider
    mmio_write(UART0_IBRD, 1);
    // Fractional part register
    mmio_write(UART0_FBRD, 40);

    // Enable FIFO & 8 bit data transmission.
    mmio_write(UART0_LCRH, (1 << 4) | (1 << 5) | (1 << 6));

    // Mask all interrupts.
    mmio_write(UART0_IMSC, (1 << 1) | (1 << 4) | (1 << 5) | (1 << 6) |
	                       (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10));

    // Enable UART0, receive & transfer part of UART.
    mmio_write(UART0_CR, (1 << 0) | (1 << 8) | (1 << 9));
}

void uart_putc(unsigned char c)
{
    while (mmio_read(UART0_FR) & (1 << 5)) {}
    mmio_write(UART0_DR, c);
}

unsigned char uart_getc()
{
    while (mmio_read(UART0_FR) & (1 << 4)) {}
    return mmio_read(UART0_DR);
}

void uart_puts(const char* str)
{
    for (size_t i = 0; str[i] != '\0'; i++)
        uart_putc((unsigned char) str[i]);
}

#if defined(__cplusplus)
extern "C" // Use C linkage for kernel_main.
#endif


void kernel_main(uint64_t dtb_ptr32, uint64_t x1, uint64_t x2, uint64_t x3)
{
    uart_init();
    uart_puts("Hello, kernel!\r\n");

    while (1)
        uart_putc(uart_getc());
}

