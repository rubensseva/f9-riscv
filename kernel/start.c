/* Copyright (c) 2013, 2014 The F9 Microkernel Project. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/*
 * __l4_start initializes microcontroller
 */

// #include INC_PLAT(gpio.h)
// #include INC_PLAT(rcc.h)

#include <platform/irq.h>
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
#include <lib/stdio.h>
#include <lib/string.h>
#include <config.h>

static char banner[] =
  "\n"
  "====================================================\n"
  " Copyright(C) 2013-2014 The F9 Microkernel Project  \n"
  "====================================================\n"
  "Git head: " GIT_HEAD "\n"
  "Host: " MACH_TYPE "\n"
  "Build: "  BUILD_TIME "\n"
  "\n";


// set up to receive timer interrupts in machine mode,
// which arrive at timervec in kernelvec.S,
// which turns them into software interrupts for
// devintr() in trap.c.
void
irqinit()
{
  // each CPU has a separate source of timer interrupts.
  int id = r_mhartid();

  // ask the CLINT for a timer interrupt.
  int interval = 1000000; // cycles; about 1/10th second in qemu.
  *(uint64*)CLINT_MTIMECMP(id) = *(uint64*)CLINT_MTIME + interval;


  // TODO: Only the timer interrupt uses scratch currently.
  // Is that ok? Do other interrupts corrupt scratch?
  //
  // prepare information in scratch[] for timervec.
  // scratch[0..2] : space for timervec to save registers.
  // scratch[3] : address of CLINT MTIMECMP register.
  // scratch[4] : desired interval (in cycles) between timer interrupts.
  uint64 *scratch = &timer_scratch[id][0];
  scratch[3] = CLINT_MTIMECMP(id);
  scratch[4] = interval;
  w_mscratch((uint64)scratch);

  // set the machine-mode trap handler.
  // w_mtvec((uint64)timervec);
  w_mtvec((uint64)kernelvec);

  // enable machine-mode interrupts.
  w_mstatus(r_mstatus() | MSTATUS_MIE);

  // enable machine-mode timer interrupts.
  w_mie(r_mie() | MIE_MTIE);
}


int main(void)
{
  run_init_hook(INIT_LEVEL_PLATFORM_EARLY);


  // from xv6

  // disable paging for now.
  w_satp(0);

  // delegate all interrupts and exceptions to supervisor mode.
  // I think most interrupts can be delegated to supervisor mode,
  // with the exceptionof timer interrupts and some way to do
  // system calls
  /* w_medeleg(0xffff); */
  /* w_mideleg(0xffff); */
  /* w_sie(r_sie() | SIE_SEIE | SIE_STIE | SIE_SSIE); */

  // configure Physical Memory Protection to give supervisor mode
  // access to all of physical memory.
  w_pmpaddr0(0x3fffffffffffffull);
  w_pmpcfg0(0xf);

  // ask for clock interrupts.
  irqinit();

  // keep each CPU's hartid in its tp register, for cpuid().
  int id = r_mhartid();
  w_tp(id);

  // done with xv6

  run_init_hook(INIT_LEVEL_PLATFORM);

  __l4_printf("%s", banner);

  run_init_hook(INIT_LEVEL_KERNEL_EARLY);

  run_init_hook(INIT_LEVEL_KERNEL);

  /* Not creating kernel thread here because it corrupts current stack
   */
  create_idle_thread();
  create_root_thread();

  ktimer_event_create(64, ipc_deliver, NULL);

  mpu_enable(MPU_ENABLED);

  run_init_hook(INIT_LEVEL_LAST);

  switch_to_kernel():

  /* Not reached */
  return 0;
}

void __l4_start(void)
{
  run_init_hook(INIT_LEVEL_EARLIEST);

  /* Fill bss with zeroes */
  memset(&bss_start, 0,
         (&bss_end - &bss_start) * sizeof(uint32_t));
  memset(&kernel_ahb_start, 0,
         (&bss_end - &bss_start) * sizeof(uint32_t));
  memset(&user_bss_start, 0,
         (&user_bss_end - & user_bss_start) * sizeof(uint32_t));

  sys_clock_init();

  /* entry point */
  main();
}
