

function asm_64 () {
    aarch64-none-elf-as -c src/bootload/boot-aarch64.S -o tmp/boot.o
    aarch64-none-elf-gcc -ffreestanding \
        -c src/kernel.c -o tmp/kernel.o -O2 -Wall -Wextra
    aarch64-none-elf-gcc -T src/linker64.ld -o tmp/kernel.elf \
        -ffreestanding -O2 -nostdlib tmp/boot.o tmp/kernel.o -lgcc
    aarch64-none-elf-objcopy tmp/kernel.elf -O binary bin/kernel-aarch64.img 
}

asm_64

