PROJECT_DIR=/run/media/purpflow/Elements/Sources/kpurple

cd $PROJECT_DIR

function asm_64 () {
    AARCH64_TOOLS=/run/media/purpflow/Elements/Applications/Linux/gcc-arm-10.3-2021.07-x86_64-aarch64-none-elf/bin

    $AARCH64_TOOLS/aarch64-none-elf-as -c src/bootload/boot-aarch64.S -o tmp/boot.o
    $AARCH64_TOOLS/aarch64-none-elf-gcc -ffreestanding \
        -c src/kernel.c -o tmp/kernel.o -O2 -Wall -Wextra
    $AARCH64_TOOLS/aarch64-none-elf-gcc -T src/linker64.ld -o tmp/kernel.elf \
        -ffreestanding -O2 -nostdlib tmp/boot.o tmp/kernel.o -lgcc
    $AARCH64_TOOLS/aarch64-none-elf-objcopy tmp/kernel.elf -O binary bin/kernel-aarch64.img 
}

asm_64

