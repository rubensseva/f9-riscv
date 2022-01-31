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

// https://github.com/riscv/riscv-plic-spec/blob/master/riscv-plic.adoc#interrupt-pending-bits
int plic_is_interrupt_pending(int irq) {
  uint32_t bit_i = irq % 32;
  uint32_t bit_mask = 1 << bit_i;
  uint32_t word_i = irq / 32;
  // The pending bits are layed out as 32 bit registers
  uint32_t *pending = (uint32_t *)PLIC_PENDING;
  uint32_t word = pending[word_i];
  return word | bit_mask;
}

void plic_clear_pending(int irq) {
  uint32_t bit_i = irq % 32;
  uint32_t bit_mask = 1 << bit_i;
  uint32_t word_i = irq / 32;
  // The pending bits are layed out as 32 bit registers

  uint32_t *pending = (uint32_t *)PLIC_PENDING;
  pending[word_i] &= ~bit_mask;
}
