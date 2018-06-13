#include "bbl.h"
#include "mtrap.h"

void boot_loader(uintptr_t dtb)
{
  putstring("Hello World!\n");
  poweroff(0);
}
