CC_DIR = $(HOME)/opt/cross/bin
CC = $(CC_DIR)/aarch64-none-elf-gcc
LD = $(CC_DIR)/aarch64-none-elf-ld
OC = $(CC_DIR)/aarch64-none-elf-objcopy

OBJ_DIR = tmp
TARGET_DIR = bin

C_SRCS   = $(shell find src -name *.c)
ASM_SRCS = $(shell find src -name *.S)
C_OBJS   = $(patsubst %.c, $(OBJ_DIR)/%.o, $(C_SRCS))
ASM_OBJS = $(patsubst %.S, $(OBJ_DIR)/%.o, $(ASM_SRCS))
FONTS_OBJS = tmp/font_psf.o tmp/font_sfn.o
CFLAGS = -Wall -O2 -ffreestanding -nostdlib -nostartfiles -I include/
LDFLAGS = -nostdlib

all: fonts $(C_OBJS) $(ASM_OBJS)
	mkdir $(TARGET_DIR) >/dev/null 2>/dev/null || true
	$(LD) $(LDFLAGS) $(C_OBJS) $(ASM_OBJS) $(FONTS_OBJS) -T src/linker0.ld -o $(TARGET_DIR)/kernel.elf
	$(OC) -O binary $(TARGET_DIR)/kernel.elf $(TARGET_DIR)/kernel-aarch64.img

$(OBJ_DIR)/%.o: %.c
	mkdir -p $(@D) >/dev/null 2>/dev/null || true
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: %.S
	mkdir -p $(@D) >/dev/null 2>/dev/null || true
	$(CC) $(CFLAGS) -c $< -o $@

fonts:
	$(LD) -r -b binary -o tmp/font_psf.o res/fonts/font.psf
	$(LD) -r -b binary -o tmp/font_sfn.o res/fonts/font.sfn

clean:
	rm $(shell find . -name *.o) 2>/dev/null || true
	rm -rf tmp/* >/dev/null 2>/dev/null || true
	rm $(TARGET_DIR)/* >/dev/null 2>/dev/null || true

run:
	qemu-system-aarch64 \
		-M raspi3b \
		-kernel bin/kernel-aarch64.img \
		-serial stdio \
		-m 1024 \
		-drive file=res/test.dd,if=sd,format=raw
