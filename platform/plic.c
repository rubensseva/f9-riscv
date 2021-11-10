#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"

int cpuid()
{
  int id = r_tp();
  return id;
}
//
// the riscv Platform Level Interrupt Controller (PLIC).
//

void plicinit(void)
{
  // set desired IRQ priorities non-zero (otherwise disabled).
  *(uint32*)(PLIC + UART0_IRQ*4) = 1;

  // This is not needed yet, I think it was only for the filesystem in xv6
  // *(uint32*)(PLIC + VIRTIO0_IRQ*4) = 1;
}

void plicinithart(void)
{
  int hart = cpuid();

  // Moved from S to M
  // Moved virtio
  // set uart's enable bit for this hart's S-mode.
  *(uint32*)PLIC_MENABLE(hart)= (1 << UART0_IRQ);
  // *(uint32*)PLIC_SENABLE(hart)= (1 << UART0_IRQ) | (1 << VIRTIO0_IRQ);

  // set this hart's S-mode priority threshold to 0.
  *(uint32*)PLIC_MPRIORITY(hart) = 0;
  // *(uint32*)PLIC_SPRIORITY(hart) = 0;
}

// ask the PLIC what interrupt we should serve.
int plic_claim(void)
{
  int hart = cpuid();

  // Moved from S to M
  int irq = *(uint32*)PLIC_MCLAIM(hart);
  // int irq = *(uint32*)PLIC_SCLAIM(hart);
  return irq;
}

// tell the PLIC we've served this IRQ.
void plic_complete(int irq)
{
  int hart = cpuid();

  // Moved from S to M
  *(uint32*)PLIC_MCLAIM(hart) = irq;
  //*(uint32*)PLIC_SCLAIM(hart) = irq;
}
