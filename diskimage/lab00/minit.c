#include "mtrap.h"
#include "atomic.h"
#include "vm.h"
#include "bits.h"
#include "uart.h"
#include "uart16550.h"
#include "finisher.h"
#include "htif.h"
#include "fdt.h"
#include <string.h>
#include <limits.h>

hls_t* hls_init(uintptr_t id)
{
  hls_t* hls = OTHER_HLS(id);
  memset(hls, 0, sizeof(*hls));
  return hls;
}
