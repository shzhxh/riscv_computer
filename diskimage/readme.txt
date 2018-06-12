This archive contains a boot loader (riscv-pk), Linux kernels and root
filesystems for the riscvemu project. The required patches to build
riscv-pk and the Linux kernel are in the 'patches' directory.

Usage:

- 32 bit RISCV emulation:

riscvemu -b 32 root-riscv32.cfg

- 64 bit RISCV emulation:

riscvemu root-riscv64.cfg

- 128 bit RISCV emulation (the boot loader switches XLEN to 64 bits
  but the emulator still emulates a 128 bit CPU):

riscvemu -b 128 root-riscv128.cfg

- 128 bit RISCV emulation test with XLEN change

riscvemu -b 128 rv128test/rv128test.cfg

Note:

- riscvemu only supports raw boot loader images. So after building
  riscv-pk, you must convert the ELF image to a raw image with:

  riscv64-unknown-linux-gnu-objcopy -O binary bbl bbl.bin
