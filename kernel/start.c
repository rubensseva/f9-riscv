/* Copyright (c) 2013, 2014 The F9 Microkernel Project. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/*
 * __l4_start initializes microcontroller
 */

#include <irq.h>
#include <error.h>
#include <types.h>
#include <riscv.h>
#include <ipc.h>
#include <ktimer.h>
#include <softirq.h>
#include <syscall.h>
#include <systhread.h>
#include <ksym.h>
#include <init_hook.h>
#include <lib/string.h>
#include <config.h>
#include <memlayout.h>
#include <mpu.h>
#include <thread.h>
#include <interrupt.h>
#include <kernel_vec_in_c.h>
#include <uart.h>
#include <plic.h>


__attribute__ ((aligned (16))) char stack0[16384];

// From kernelvec.S
extern void kernelvec();

void
irqinit()
{
  // ask the CLINT for a timer interrupt.
  uint32_t interval = 1000000; // cycles; about 1/10th second in qemu.
  *(uint32_t*)CLINT_MTIME = 0;
  *(uint32_t*)CLINT_MTIMECMP = *(uint32_t*)CLINT_MTIME + interval;

  // enable supervisor interrupts
  // NOTE: I dont think these are necessary at all?
  // Should be enough with mstatus_mie, and u-mode.
  // w_sie(r_sie() | SIE_SEIE | SIE_STIE | SIE_SSIE);
  /* w_sie(r_sie() | SIE_SEIE | SIE_SSIE); */
  /* w_mstatus(r_mstatus() | MSTATUS_SIE); */

  w_mtvec((uint32_t)kernelvec);

  // enable machine-mode interrupts.
  // this should be enabled after the first interrupt
  // w_mstatus(r_mstatus() | MSTATUS_MIE);

  // when we execute mret, mstatus.mie is set to mstatus.mpie. Therefore, we need to set
  // mstatus.mpie as well here, to prevent mstatus.mie from being disabled.
  w_mstatus(r_mstatus() | MSTATUS_MPIE);
  // enable m-mode timer interrupts, u-mode software interrupts, and u-mode external interrupts
  w_mie(r_mie() | MIE_MTIE | MIE_UEIE | MIE_USIE);
}


int main(void)
{
  // Disable paging
  w_satp(0);

  // Init PMP
  w_pmpcfg0(0x08080808);
  w_pmpcfg1(0x08080808);
  w_pmpcfg2(0x08080808);
  w_pmpcfg3(0x08080808);

  // Init ktables
  thread_init_ktable();
  user_irq_init_ktable();
  as_t_init_ktable();
  ktimer_init_ktable();
  fpage_table_init_ktable();

  // Init subsystems
  sched_init();
  syscall_init();
  ktimer_event_init();
  memory_init();

  ktimer_event_create(64, ipc_deliver, NULL);

  thread_init_subsys();
  create_idle_thread();
  create_root_thread();

  create_kernel_thread();
  current = get_kernel_thread();

  // Init user interrupts
  interrupt_init();

  irqinit();
  plicinit();
  plicinithart();
  uartinit();

  switch_to_kernel();

  /* Not reached */
  return 0;
}

extern void __l4_start(void)
{

  /* Fill bss with zeroes */
  memset(&bss_start, 0,
         (&bss_end - &bss_start) * sizeof(uint32_t));

  /* entry point */
  main();
}
