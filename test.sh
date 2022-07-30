./build.sh

qemu-system-aarch64 -M raspi3b -serial stdio -kernel bin/kernel-aarch64.img

