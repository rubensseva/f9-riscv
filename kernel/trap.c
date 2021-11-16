
#include <stdint.h>
#include <syscall.h>
#include <memlayout.h>
#include <ktimer.h>
#include <syscall.h>
#include <riscv.h>
#include <plic.h>

void no_interrupt(void) {
  // do nothing
}

extern void timervec();

void machine_timer_interrupt_handler(void) {

  // get the address of clint_mtimecmp.
  // use 0 as the hart id, since we currently assume only
  // one hard
  uint64_t *clint_addr = (uint64_t*)CLINT_MTIMECMP;
  uint32_t interval = 10000;
  uint64_t new_val = *clint_addr + interval;
  *clint_addr = new_val;

  ktimer_handler();
}

void ecall_from_s_handler(void) {
  // simply call old syscall handler
  svc_handler();
}

void supervisor_external_interrupt(void) {
    int irq = plic_claim();

    if(irq == UART0_IRQ){
      // uartintr();
    } else if(irq == VIRTIO0_IRQ){
      // virtio_disk_intr();
    } else if(irq){
      // printf("unexpected interrupt irq=%d\n", irq);
    }

    // the PLIC allows each device to raise at most one
    // interrupt at a time; tell the PLIC the device is
    // now allowed to interrupt again.
    if(irq)
      plic_complete(irq);
}

void (*async_handler[12])() = {
  no_interrupt,
  no_interrupt,
  no_interrupt,
  no_interrupt,
  no_interrupt,
  no_interrupt,
  machine_timer_interrupt_handler,
  no_interrupt,
  no_interrupt,
  supervisor_external_interrupt, // Interrupts to PLIC goes here
  no_interrupt,
  no_interrupt,
};
void (*sync_handler[16])() = {
  no_interrupt,
  no_interrupt,
  no_interrupt,
  no_interrupt,
  no_interrupt,
  no_interrupt,
  no_interrupt,
  no_interrupt,
  no_interrupt,
  ecall_from_s_handler,
  no_interrupt,
  no_interrupt,
  no_interrupt,
  no_interrupt,
  no_interrupt,
  no_interrupt,
};

#define MCAUSE_INT_MASK 0x80000000 // [31]=1 interrupt, else exception
#define MCAUSE_CODE_MASK 0x7FFFFFFF // low bits show code

extern void kerneltrap()
{
  unsigned long mcause_value = r_mcause();
  // this works for everything except PLIC
  if (mcause_value & MCAUSE_INT_MASK) {
    // Branch to interrupt handler here
    // Index into 32-bit array containing addresses of functions
    async_handler[(mcause_value & MCAUSE_CODE_MASK)]();
  } else {
    // Branch to exception handler
    sync_handler[(mcause_value & MCAUSE_CODE_MASK)]();
  }
}
