PROJECT_DIR=/run/media/purpflow/Elements/Sources/kpurple/
RPI=2

cd $PROJECT_DIR

function asm_32 () {
    arm-none-eabi-gcc -mcpu=$1 -fpic -ffreestanding \
        -c src/bootload/boot-$2.S -o tmp/boot.o
    arm-none-eabi-gcc -mcpu=$1 -fpic -ffreestanding -std=gnu99 \
        -c src/kernel.c -o tmp/kernel.o \
        -O2 -Wall -Wextra
    arm-none-eabi-gcc -T src/linker32.ld -o tmp/kernel.elf -ffreestanding -O2 -nostdlib \
        tmp/boot.o tmp/kernel.o -lgcc
    arm-none-eabi-objcopy tmp/kernel.elf -O binary bin/kernel-$1.img1
}

function asm_64 () {
    AARCH64_TOOLS=/run/media/purpflow/Elements/Applications/Linux/\
        gcc-arm-10.3-2021.07-x86_64-aarch64-none-elf/bin

    $AARCH64_TOOLS/aarch64-none-elf-as -c src/bootload/boot-3.S -o tmp/boot.o
    $AARCH64_TOOLS/aarch64-none-elf-gcc -ffreestanding -c \
        src/kernel.c -o tmp/kernel.o -O2 -Wall -Wextra
    
}

if [[ $RPI -eq 1 ]]; then
    asm_32 arm1176jzf-s 1
elif [[ $RPI -eq 2 ]]; then
    asm_32 cortex-a7 2
else
    asm_64
fi

