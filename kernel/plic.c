#include "types.h"
#include "memlayout.h"
#include "riscv.h"


void plicinit(void)
{
  // set desired IRQ priorities non-zero (otherwise disabled).
  *(uint32_t*)(PLIC + UART0_IRQ*4) = 1;
  // *(uint32_t*)(PLIC + VIRTIO0_IRQ*4) = 1;
}

void plicinithart(void)
{
  // set uart's enable bit for this hart's S-mode.
  *(uint32_t*)PLIC_SENABLE = (1 << UART0_IRQ);
  // set this hart's S-mode priority threshold to 0.
  *(uint32_t*)PLIC_SPRIORITY = 0;
}

// ask the PLIC what interrupt we should serve.
int plic_claim(void)
{
  int irq = *(uint32_t*)PLIC_SCLAIM;
  return irq;
}

// tell the PLIC we've served this IRQ.
void plic_complete(int irq)
{
  *(uint32_t*)PLIC_SCLAIM = irq;
}
