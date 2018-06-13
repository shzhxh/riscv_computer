#include "mtrap.h"
#include "htif.h"
#include "uart.h"
#include "uart16550.h"
#include "finisher.h"


static uintptr_t mcall_console_putchar(uint8_t ch)
{
#if 0
  if (uart) {
    uart_putchar(ch);
  } else if (uart16550) {
    uart16550_putchar(ch);
  } else if (htif) {
#endif
    htif_console_putchar(ch);
//  }
  return 0;
}

void poweroff(uint16_t code)
{
  putstring("Power off\n");
  finisher_exit(code);
  if (htif) {
    htif_poweroff();
  } else {
    while (1) { asm volatile ("#noop\n"); }
  }
}

void putstring(const char* s)
{
    int c;
    while (*s) {
        c = *s++;
        if (c == '\n')
            mcall_console_putchar('\r');
        mcall_console_putchar(c);
    }
}
