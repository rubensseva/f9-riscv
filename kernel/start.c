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


__attribute__ ((aligned (16))) char stack0[4096];

// From kernelvec.S
extern void kernelvec();

void
irqinit()
{
  // ask the CLINT for a timer interrupt.
  uint64_t interval = 1000000; // cycles; about 1/10th second in qemu.
  *(uint64_t*)CLINT_MTIMECMP = *(uint64_t*)CLINT_MTIME + interval;

  // disable paging for now.
  w_satp(0);

  // delegate all exceptions except s-mode ecalls to supervisor mode.
  w_medeleg(0xffff & (1L << 9));
  // delegate all interrupts except m-mode timer interrupts to supervisor mode
  w_mideleg(0xffff & (1L << 7));

  // enable supervisor interrupts
  w_sie(r_sie() | SIE_SEIE | SIE_STIE | SIE_SSIE);

  // set the machine-mode trap handler.
  w_mtvec((uint64_t)kernelvec);

  // enable machine-mode interrupts.
  // this is not necessary, machine mode interrupts are always enabled, the bits are read only.
  // w_mstatus(r_mstatus() | MSTATUS_MIE);

  // enable machine-mode timer interrupts.
  w_mie(r_mie() | MIE_MTIE);
}


int main(void)
{
  // run_init_hook(INIT_LEVEL_PLATFORM_EARLY);

  // disable paging for now.
  w_satp(0);

  // configure Physical Memory Protection to give supervisor mode
  // access to all of physical memory.
  w_pmpaddr0(0x3fffffffffffffull);
  w_pmpcfg0(0xf);

  irqinit();

  // user_irq_table.data = kt_user_irq_table_data;
  /* as_t.data = kt_as_t_data; */
  /* ktimer_event_t.data = kt_ktimer_event_t_data; */
  /* tcb_t.data = kt_tcb_t_data; */
  /* fpage_t.data  = kt_fpage_t_data; */

  // initialize ktables
  thread_init_ktable();
  user_irq_init_ktable();
  ktimer_init_ktable();
  as_t_init_ktable();
  thread_init_ktable();
  fpage_table_init_ktable();

  // run_init_hook(INIT_LEVEL_PLATFORM);

  // run_init_hook(INIT_LEVEL_KERNEL_EARLY);

  // run_init_hook(INIT_LEVEL_KERNEL);

  /* Not creating kernel thread here because it corrupts current stack
   */
  create_idle_thread();
  create_root_thread();

  ktimer_event_create(64, ipc_deliver, NULL);

  // run_init_hook(INIT_LEVEL_LAST);

  switch_to_kernel();

  /* Not reached */
  return 0;
}

//__attribute__((section("__l4_start_section"))) extern void __l4_start(void)
extern void __l4_start(void)
{
  // run_init_hook(INIT_LEVEL_EARLIEST);

  /* Fill bss with zeroes */
  memset(&bss_start, 0,
         (&bss_end - &bss_start) * sizeof(uint32_t));

  /* entry point */
  main();
}
