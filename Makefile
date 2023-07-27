SRCS = $(shell find src -name *.c)
OBJS = $(SRCS:.c=.o)
CFLAGS = -Wall -O2 -ffreestanding -nostdlib -nostartfiles
CC_DIR = $(HOME)/opt/cross/bin

all: clean kernel-aarch64.img

tmp/boot.o: src/bootload/boot-aarch64.S
	$(CC_DIR)/aarch64-none-elf-gcc $(CFLAGS) -c src/bootload/boot-aarch64.S -o tmp/boot.o

%.o: %.c
	$(CC_DIR)/aarch64-none-elf-gcc $(CFLAGS) -c $< -o $@

kernel-aarch64.img: tmp/boot.o $(OBJS)
	$(CC_DIR)/aarch64-none-elf-ld -nostdlib tmp/boot.o $(OBJS) -T src/linker64.ld -o bin/kernel.elf
	$(CC_DIR)/aarch64-none-elf-objcopy -O binary bin/kernel.elf bin/kernel-aarch64.img

clean:
	for obj in $(OBJS); do \
		rm $$obj; \
	done;
	rm bin/kernel.elf >/dev/null 2>/dev/null || true

run:
	qemu-system-aarch64 -M raspi3b -kernel bin/kernel-aarch64.img -serial stdio