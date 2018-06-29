#ifndef _RISCV_HTIF_H
#define _RISCV_HTIF_H

#include <stdint.h>

# define TOHOST_CMD(dev, cmd, payload) \
  (((uint64_t)(dev) << 56) | ((uint64_t)(cmd) << 48) | (uint64_t)(payload))

#define FROMHOST_CMD(fromhost_value) ((uint64_t)(fromhost_value) << 8 >> 56)
#define FROMHOST_DATA(fromhost_value) ((uint64_t)(fromhost_value) << 16 >> 16)

extern uintptr_t htif;
void htif_console_putchar(uint8_t);
void htif_poweroff() __attribute__((noreturn));

#endif
