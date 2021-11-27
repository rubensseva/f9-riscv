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


__attribute__ ((aligned (16))) char stack0[16384];

// From kernelvec.S
extern void kernelvec();

void
irqinit()
{
  // ask the CLINT for a timer interrupt.
  uint64_t interval = 1000000; // cycles; about 1/10th second in qemu.
  *(uint64_t*)CLINT_MTIMECMP = *(uint64_t*)CLINT_MTIME + interval;

  // disable paging
  w_satp(0);

  // enable supervisor interrupts
  w_sie(r_sie() | SIE_SEIE | SIE_STIE | SIE_SSIE);
  w_mstatus(r_mstatus() | MSTATUS_SIE);

  w_mtvec((uint64_t)kernel_vec_in_c);

  // enable machine-mode interrupts.
  w_mstatus(r_mstatus() | MSTATUS_MIE);
  // when we execute mret, mstatus.mie is set to mstatus.mpie. Therefore, we need to set
  // mstatus.mpie as well here, to prevent mstatus.mie from being disabled.
  w_mstatus(r_mstatus() | MSTATUS_MPIE);
  // enable machine-mode timer interrupts.
  w_mie(r_mie() | MIE_MTIE);
}


int main(void)
{
  // disable paging for now.
  w_satp(0);

  // configure Physical Memory Protection to give supervisor mode
  // access to all of physical memory.
  w_pmpaddr0(0x3fffffffffffffull);
  w_pmpcfg0(0xf);

  irqinit();
  sched_init();
  syscall_init();
  ktimer_event_init();

  // Not creating kernel thread here because it corrupts current stack
  thread_init_subsys();
  create_idle_thread();
  create_root_thread();

  ktimer_event_create(64, ipc_deliver, NULL);

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
