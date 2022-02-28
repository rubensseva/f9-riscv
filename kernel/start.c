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
#include <uart_ESP32_C3.h>
#include <ESP32_C3.h>
#include <plic.h>
#include <stdio.h>
#include <debug.h>

// TODO: Shouldnt really be here
extern dbg_layer_t dbg_layer;

static char banner[] =
	"\n"
	"====================================================\n"
	" Copyright(C) 2013-2014 The F9 Microkernel Project  \n"
	"====================================================\n"
	"Git head: " GIT_HEAD "\n"
	"Host: " MACH_TYPE "\n"
	"Build: "  BUILD_TIME "\n"
	"\n";

__attribute__ ((aligned (16))) char stack0[16384];

// From kernelvec.S
extern void kernelvec();
extern void _vector_table();

void irqinit() {
  // ask the CLINT for a timer interrupt.
  /* uint32_t interval = 1000000; // cycles; about 1/10th second in qemu. */
  /* *(uint32_t*)CLINT_MTIME = 0; */
  /* *(uint32_t*)CLINT_MTIMECMP = *(uint32_t*)CLINT_MTIME + interval; */

  // enable supervisor interrupts
  // NOTE: I dont think these are necessary at all?
  // Should be enough with mstatus_mie, and u-mode.
  // w_sie(r_sie() | SIE_SEIE | SIE_STIE | SIE_SSIE);
  /* w_sie(r_sie() | SIE_SEIE | SIE_SSIE); */
  /* w_mstatus(r_mstatus() | MSTATUS_SIE); */

  w_mtvec((uint32_t)_vector_table);

  // enable machine-mode interrupts.
  // this should be enabled after the first interrupt
  // w_mstatus(r_mstatus() | MSTATUS_MIE);

  // when we execute mret, mstatus.mie is set to mstatus.mpie. Therefore, we need to set
  // mstatus.mpie as well here, to prevent mstatus.mie from being disabled.
  w_mstatus(r_mstatus() | MSTATUS_MPIE);
  // enable m-mode timer interrupts, u-mode software interrupts, and u-mode external interrupts
  // I dont think we can do the next line on esp32_c3, doesnt seem like mie is available at all
  /* w_mie(r_mie() | MIE_MTIE | MIE_UEIE | MIE_USIE); */
}

void timerinit() {
  /* Following the steps in 10.5.3 in TRM */
  /* 1. Set SYSTIMER_TARGETx_TIMER_UNIT_SEL to select the counter (UNIT0 or UNIT1) used for COMPx. */
  uint32_t *systimer_target0_conf = SYSTEM_TIMER_BASE + SYSTIMER_TARGET0_CONF_REG;
  *systimer_target0_conf |= (1 << SYSTIMER_TARGET0_CONF_REG__SYSTIMER_TARGET0_TIMER_UNIT_SEL);
  /* 2. Set an alarm period (δt), and fill it to SYSTIMER_TARGETx_PERIOD. */
  int alarm = 1000;
  *systimer_target0_conf &= ~0x3ffffff; // zero out whatever is in period field (bits 0 - 25)
  *systimer_target0_conf |= alarm;
  /* 3. Set SYSTIMER_TIMER_COMPx_LOAD to synchronize the alarm period (δt) to COMPx, i.e. load the alarm period (δt) to COMPx. */
  uint32_t *systimer_comp0_load = SYSTEM_TIMER_BASE + SYSTIMER_COMP0_LOAD_REG;
  *systimer_comp0_load = 1;
  /* 4. Set SYSTIMER_TARGETx_PERIOD_MODE to configure COMPx into period mode. */
  *systimer_target0_conf |= (1 << SYSTIMER_TARGET0_CONF_REG__SYSTIMER_TARGET0_PERIOD_MODE);
  /* 5. Set SYSTIMER_TARGETx_WORK_EN to enable the selected COMPx. COMPx starts comparing the count value with the sum of start value + n*δt (n = 1, 2, 3...). */
  uint32_t *systimer_conf_reg = SYSTEM_TIMER_BASE + SYSTIMER_CONF_REG;
  *systimer_conf_reg |= (1 << SYSTIMER_CONF_REG__SYSTIMER_TARGET0_WORK_EN);
  /* 6. Set SYSTIMER_TARGETx_INT_ENA to enable timer interrupt. A SYSTIMER_TARGETx_INT interrupt is triggered when Unitn counts to start value + n*δt (n = 1, 2, 3...) set in step 2. */
  uint32_t *systimer_int_ena = SYSTEM_TIMER_BASE + SYSTIMER_INT_ENA_REG;
  *systimer_int_ena |= 1; // enable interrups for target 0
}


int main(void)
{
  // Disable paging
  /* w_satp(0); */

  // Init PMP
  w_pmpcfg0(0x0);
  w_pmpcfg1(0x0);
  w_pmpcfg2(0x0);
  w_pmpcfg3(0x0);
  /* w_pmpcfg0(0xFFFFFFFF); */

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

  timerinit();
  /* plicinit(); */
  /* plicinithart(); */

  // TODO: Put these things in appropriate functions
  UART_init(115200);
  dbg_layer = DL_BASIC | DL_KDB  | DL_KTABLE | DL_SOFTIRQ | DL_THREAD |
    DL_KTIMER | DL_SYSCALL | DL_SCHEDULE | DL_MEMORY | DL_IPC;
  __l4_printf("%s", banner);

  switch_to_kernel();

  /* Not reached */
  return 0;
}

extern void __l4_start(void)
{

  /* Fill bss with zeroes */
  for (char *p = &bss_start; p < &bss_end; p++) {
    *p = 0;
  }
  /* memset(&bss_start, 0, */
  /*        (&bss_end - &bss_start) * sizeof(uint32_t)); */

  /* entry point */
  main();
}
