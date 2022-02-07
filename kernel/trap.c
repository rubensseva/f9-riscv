#include <stdint.h>
#include <syscall.h>
#include <memlayout.h>
#include <ktimer.h>
#include <syscall.h>
#include <riscv.h>
#include <plic.h>
#include <irq.h>
#include <uart.h>
#include <interrupt.h>

void no_interrupt(void) {
  int x = 2 + 3;
  // do nothing
}

extern void timervec();
extern void kernel_vec_in_c_restore();
void machine_timer_interrupt_handler(void);


void supervisor_timer_interrupt_handler(void) {
  machine_timer_interrupt_handler();
}

void machine_timer_interrupt_handler(void) {

  uint32_t *clint_mtimecmp = (uint32_t*)CLINT_MTIMECMP;
  uint32_t *clint_mtime = (uint32_t*)CLINT_MTIME;
  uint32_t interval = 1000000; // cycles; about 1/10th second in qemu.
  uint32_t new_val = *clint_mtime + interval;
  *clint_mtimecmp = new_val;

  ktimer_handler();
}

void ecall_from_s_handler(void) {
  svc_handler();
}
void ecall_from_m_handler(void) {
  svc_handler();
}

void supervisor_external_interrupt(void) {
    int irq = plic_claim();

    __interrupt_handler(irq);

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
  supervisor_timer_interrupt_handler,
  no_interrupt,
  machine_timer_interrupt_handler,
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
  ecall_from_m_handler, // should only happen from kernel thread
  no_interrupt,
  no_interrupt,
  no_interrupt,
  no_interrupt,
};

#define MCAUSE_INT_MASK 0x80000000 // [31]=1 interrupt, else exception
#define MCAUSE_CODE_MASK 0x7FFFFFFF // low bits show code

extern void kerneltrap(uint32_t* caller_sp)
{
  unsigned long mcause_value = r_mcause();

  if (mcause_value & MCAUSE_INT_MASK) {
    current->ctx.mepc = r_mepc();
    async_handler[(mcause_value & MCAUSE_CODE_MASK)]();
  } else {
    current->ctx.mepc = r_mepc() + 4;
    sync_handler[(mcause_value & MCAUSE_CODE_MASK)]();
  }

  // context switch
  current->ctx.sp = (uint32_t) caller_sp;
  tcb_t* sel = schedule_select();
  if (sel != current) {
    thread_switch(sel);
  }

  /* Kernel thread should run in m-mode, rest should run in s-mode
   * The reason we need the kernel thread in m-mode is because it needs
   * the ability to disable all interrupts */
	extern tcb_t *kernel;
  unsigned long x = r_mstatus();
  x &= ~MSTATUS_MPP_MASK;
  if (thread_current() == kernel) {
    x |= MSTATUS_MPP_M;
  } else {
    x |= MSTATUS_MPP_S;
  }
  w_mstatus(x);

  // current = kernel_thread or whatevah
    // set previous privelege mode to machine mode
  // else
  // set previous privelege mode to s-mode

  // update mepc in case of context switch, or mepc + 4
  __asm__ ("csrw mepc, %0" : : "r" (current->ctx.mepc));
  __asm__ volatile ("mv a0, %0" : : "r" (current->ctx.sp));
}
