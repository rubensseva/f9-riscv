#include <stdint.h>
#include <syscall.h>
#include <memlayout.h>
#include <ktimer.h>
#include <syscall.h>
#include <riscv.h>
#include <plic.h>
#include <irq.h>
#include <interrupt.h>

void machine_timer_interrupt_handler(void);


extern void timervec();
extern void kernel_vec_in_c_restore();

void access_fault_handler(void) {
  // mtval will contain the correct faulting address. For example, on an
  // instruction access fault, it will contain the instruction. On a load access
  // fault, it will contain the address that caused that load to fail.
  uint32_t fault_addr = r_mtval();
  // Try to setup an mpu region for the address that cause the exception.
  // If the address is not in the threads address space, nothing will happen.
  if (mpu_select_lru(current->as, fault_addr) != 0) {
    // If we get here, the faulting address is not in the threads address space.
    // TODO: What should we do here? Probably send the exception as an IPC message to pager?
  }
}

/* interrupt handlers start */

void unimplemented(void) {
  // Do nothing
  int x = 2 + 3;
}

void supervisor_timer_interrupt_handler(void) {
  machine_timer_interrupt_handler();
}

/* interrupt handlers end */

/* exception handlers start */
/* access fault handlers start */
void illegal_instruction_access_fault_handler(void) {
  access_fault_handler();
}

void load_access_fault_handler(void) {
  access_fault_handler();
}

void store_or_AMO_access_fault_handler(void) {
  access_fault_handler();
}
/* access fault handlers end */

void machine_timer_interrupt_handler(void) {

  uint32_t *clint_mtimecmp = (uint32_t*)CLINT_MTIMECMP;
  uint32_t *clint_mtime = (uint32_t*)CLINT_MTIME;
  uint32_t interval = 1000000; // cycles; about 1/10th second in qemu.
  uint32_t new_val = *clint_mtime + interval;
  *clint_mtimecmp = new_val;

  ktimer_handler();
}

void ecall_from_u_handler(void) {
  current->ctx.mepc = r_mepc() + 4;
  svc_handler();
}
void ecall_from_s_handler(void) {
  current->ctx.mepc = r_mepc() + 4;
  svc_handler();
}
void ecall_from_m_handler(void) {
  current->ctx.mepc = r_mepc() + 4;
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

/* exception handlers end */

void (*interrupt_handlers[12])() = {
  unimplemented,
  unimplemented,
  unimplemented,
  unimplemented,
  unimplemented,
  supervisor_timer_interrupt_handler,
  unimplemented,
  machine_timer_interrupt_handler,
  unimplemented,
  supervisor_external_interrupt, // Interrupts to PLIC goes here
  unimplemented,
  unimplemented,
};
void (*exception_handlers[16])() = {
  unimplemented,
  illegal_instruction_access_fault_handler,
  unimplemented,
  unimplemented,
  unimplemented,
  load_access_fault_handler,
  unimplemented,
  store_or_AMO_access_fault_handler,
  ecall_from_u_handler,
  ecall_from_s_handler,
  unimplemented,
  ecall_from_m_handler, // should only happen from kernel thread
  unimplemented,
  unimplemented,
  unimplemented,
  unimplemented,
};

#define MCAUSE_INT_MASK 0x80000000 // [31]=1 interrupt, else exception
#define MCAUSE_CODE_MASK 0x7FFFFFFF // low bits show code

extern void kerneltrap(uint32_t* caller_sp)
{
  unsigned long mcause_value = r_mcause();

  current->ctx.mepc = r_mepc();
  if (mcause_value & MCAUSE_INT_MASK) {
    interrupt_handlers[(mcause_value & MCAUSE_CODE_MASK)]();
  } else {
    exception_handlers[(mcause_value & MCAUSE_CODE_MASK)]();
  }

  // Context switch
  current->ctx.sp = (uint32_t) caller_sp;
  tcb_t* sel = schedule_select();
  if (sel != current) {
    thread_switch(sel);
  }

  /* Kernel thread should run in m-mode, rest should run in u-mode
   * The reason we need the kernel thread in m-mode is because it needs
   * the ability to disable all interrupts */
  extern tcb_t *kernel;
  unsigned long x = r_mstatus();
  x &= ~MSTATUS_MPP_MASK;
  if (thread_current() == kernel) {
    x |= MSTATUS_MPP_M;
  } else {
    x |= MSTATUS_MPP_U;
  }
  w_mstatus(x);

  __asm__ ("csrw mepc, %0" : : "r" (current->ctx.mepc));
  __asm__ volatile ("mv a0, %0" : : "r" (current->ctx.sp));
}
