SRCS = $(shell find src -name *.c)
OBJS = $(SRCS:.c=.o)
CFLAGS = -Wall -O2 -ffreestanding -nostdlib -nostartfiles
CC_DIR = $(HOME)/opt/cross/bin

all: clean kernel-aarch64.img

tmp/boot.o: src/bootload/boot-aarch64.S
	$(CC_DIR)/aarch64-none-elf-gcc $(CFLAGS) -c src/bootload/boot-aarch64.S -o tmp/boot.o

%.o: %.c
	$(CC_DIR)/aarch64-none-elf-gcc $(CFLAGS) -c $< -o $@

tmp/font_psf.o: src/uart/graphics/fonts/font.psf
	$(CC_DIR)/aarch64-none-elf-ld -r -b binary -o tmp/font_psf.o src/uart/graphics/fonts/font.psf

tmp/font_sfn.o: src/uart/graphics/fonts/font.sfn
	$(CC_DIR)/aarch64-none-elf-ld -r -b binary -o tmp/font_sfn.o src/uart/graphics/fonts/font.sfn

kernel-aarch64.img: tmp/boot.o $(OBJS)
	$(CC_DIR)/aarch64-none-elf-ld -nostdlib tmp/boot.o tmp/font_psf.o tmp/font_sfn.o \
		$(OBJS) -T src/linker64.ld -o bin/kernel.elf
	$(CC_DIR)/aarch64-none-elf-objcopy -O binary bin/kernel.elf bin/kernel-aarch64.img

clean:
	for obj in $(OBJS); do \
		rm $$obj || true; \
	done;
	rm bin/kernel.elf >/dev/null 2>/dev/null || true

run:
	qemu-system-aarch64 -M raspi3b -kernel bin/kernel-aarch64.img \
	-drive file=data/test.dd,if=sd,format=raw -serial stdio