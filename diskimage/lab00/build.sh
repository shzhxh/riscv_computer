#!/bin/bash
riscv64-unknown-elf-gcc -MMD -MP -march=rv32g -mabi=ilp32d -Wall -Werror -D__NO_INLINE__ -mcmodel=medany -O2 -std=gnu99 -Wno-unused -Wno-attributes -fno-delete-null-pointer-checks  -I.  -c htif.c
riscv64-unknown-elf-gcc -MMD -MP -march=rv32g -mabi=ilp32d -Wall -Werror -D__NO_INLINE__ -mcmodel=medany -O2 -std=gnu99 -Wno-unused -Wno-attributes -fno-delete-null-pointer-checks  -I.  -c mentry.S
riscv64-unknown-elf-ar rcv --target=elf32-littleriscv libmachine.a htif.o mentry.o
riscv64-unknown-elf-ranlib --target=elf32-littleriscv libmachine.a
riscv64-unknown-elf-gcc -MMD -MP -march=rv32g -mabi=ilp32d -Wall -Werror -D__NO_INLINE__ -mcmodel=medany -O2 -std=gnu99 -Wno-unused -Wno-attributes -fno-delete-null-pointer-checks  -I.  -c bbl.c
riscv64-unknown-elf-gcc -march=rv32g -mabi=ilp32d -nostartfiles -nostdlib -static  -o bbl bbl.o -L.  -lmachine -lgcc -T bbl.lds
riscv64-unknown-elf-objcopy -O binary bbl bbl.bin
riscv64-unknown-elf-objdump -S bbl > bbl.s
