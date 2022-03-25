/* Copyright (c) 2013, 2014 The F9 Microkernel Project. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <irq.h>
#include <error.h>
#include <types.h>
#include <riscv.h>
#include <ipc.h>
#include <fpage_impl.h>
#include <ktimer.h>
#include <softirq.h>
#include <syscall.h>
#include <systhread.h>
#include <ksym.h>
#include <init_hook.h>
#include <lib/string.h>
#include <config.h>
#include <mpu.h>
#include <thread.h>
#include <interrupt.h>
#include <uart_ESP32_C3.h>
#include <ESP32_C3.h>
#include <utility.h>
#include <stdio.h>
#include <debug.h>

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

/* From vector_table_to_direct */
extern void _vector_table();

void irq_init() {
    w_mtvec((uint32_t)_vector_table);

    /* When we execute mret, mstatus.mie is set to mstatus.mpie. Therefore, we
       need to set mstatus.mpie as well here, to prevent mstatus.mie from being
       disabled. */
    w_mstatus(r_mstatus() | MSTATUS_MPIE);
}

void system_timer_init() {
    /* Following the steps in 10.5.3 in TRM */
    /* 1. Set SYSTIMER_TARGETx_TIMER_UNIT_SEL to select the counter (UNIT0 or UNIT1) used for COMPx. */
    /* NOTE: We will set this to 0, which I believe will select UNIT0 */
    volatile uint32_t *systimer_target0_conf = REG(SYSTEM_TIMER_BASE + SYSTIMER_TARGET0_CONF_REG);
    *systimer_target0_conf &= ~(1 << SYSTIMER_TARGET0_CONF_REG__SYSTIMER_TARGET0_TIMER_UNIT_SEL);
    /* 2. Set an alarm period (δt), and fill it to SYSTIMER_TARGETx_PERIOD. */
    int alarm = 16000000;
    *systimer_target0_conf &= ~0x3ffffff; // zero out whatever is in period field (bits 0 - 25)
    *systimer_target0_conf |= alarm;
    /* 3. Set SYSTIMER_TIMER_COMPx_LOAD to synchronize the alarm period (δt) to COMPx, i.e. load the alarm period (δt) to COMPx. */
    volatile uint32_t *systimer_comp0_load = REG(SYSTEM_TIMER_BASE + SYSTIMER_COMP0_LOAD_REG);
    *systimer_comp0_load = 1;
    /* 4. Set SYSTIMER_TARGETx_PERIOD_MODE to configure COMPx into period mode. */
    *systimer_target0_conf |= (1 << SYSTIMER_TARGET0_CONF_REG__SYSTIMER_TARGET0_PERIOD_MODE);
    /* 5. Set SYSTIMER_TARGETx_WORK_EN to enable the selected COMPx. COMPx starts comparing the count value with the sum of start value + n*δt (n = 1, 2, 3...). */
    volatile uint32_t *systimer_conf_reg = REG(SYSTEM_TIMER_BASE + SYSTIMER_CONF_REG);
    *systimer_conf_reg |= (1 << SYSTIMER_CONF_REG__SYSTIMER_TARGET0_WORK_EN);
    /* 6. Set SYSTIMER_TARGETx_INT_ENA to enable timer interrupt. A SYSTIMER_TARGETx_INT interrupt is triggered when Unitn counts to start value + n*δt (n = 1, 2, 3...) set in step 2. */
    volatile uint32_t *systimer_int_ena = REG(SYSTEM_TIMER_BASE + SYSTIMER_INT_ENA_REG);
    *systimer_int_ena |= 1; // enable interrups for target 0

    /* Map peripheral interrupt system timer to a CPU interrupt number */
    volatile uint32_t *interrupt_core0_systimer_target0_int_map_reg = REG(INTERRUPT_MATRIX_BASE + INTERRUPT_CORE0_SYSTIMER_TARGET0_INT_MAP_REG);
    *interrupt_core0_systimer_target0_int_map_reg = CONFIG_SYSTEM_TIMER_CPU_INTR;

    /* Set interrupt type to level-type interrupt, as stated in TRM 10.4.4. */
    volatile uint32_t *interrupt_core0_cpu_int_type = REG(INTERRUPT_MATRIX_BASE + INTERRUPT_CORE0_CPU_INT_TYPE_REG);
    *interrupt_core0_cpu_int_type &= ~(1 << CONFIG_SYSTEM_TIMER_CPU_INTR); // set to level type by writing zero

    /* Set the priority of the interrupt */
    volatile uint32_t *interrupt_core0_cpu_int_pri = REG(INTERRUPT_MATRIX_BASE + INTERRUPT_CORE0_CPU_INT_PRI_n_REG + 0x4 * (CONFIG_SYSTEM_TIMER_CPU_INTR - 1));
    *interrupt_core0_cpu_int_pri = 10; // Set hightes priority for now TODO: Set a more sensible priority

    /* Set the threshold of interrupt priorities to 1, so that all interrupts are taken except those with priority = 0 */
    /* TODO: This should be in a more general interrupt_init() type of function */
    volatile uint32_t *interrupt_core0_cpu_int_thresh = REG(INTERRUPT_MATRIX_BASE + INTERRUPT_CORE0_CPU_INT_THRESH_REG);
    *interrupt_core0_cpu_int_thresh = 1;

    /* After mapping system timer to CPU interrupt, we enable the CPU interrupt number */
    volatile uint32_t *interrupt_core0_cpi_in_enable = REG(INTERRUPT_MATRIX_BASE + INTERRUPT_CORE0_CPU_INT_ENABLE_REG);
    *interrupt_core0_cpi_in_enable |= (1 << CONFIG_SYSTEM_TIMER_CPU_INTR);


    /* And finally start the counter */
    *systimer_conf_reg |= (1 << SYSTIMER_CONF_REG__SYSTIMER_TIMER_UNIT0_WORK_EN);

}

/**
   Map the interrupt vector to a pmp fpage. This is needed when an interrupt happens in
   user mode, otherwise it triggers an illegal instruction access exception.
 */
void map_intr_vector() {
    w_pmpaddr14((int)_vector_table >> 2);
    w_pmpaddr15(((int)_vector_table + (4 * 32)) >> 2); // We have 32 instructions in interrupt vector, each one is 4 bytes long

    uint32_t old_cfg = r_pmpcfg3();
    uint32_t new_cfg = old_cfg & (0x0000ffff);
    new_cfg |= 0x8C008000; // write 0b10001100 10000000 to set TOR mode, exec-only, and lock bit on both pmpaddr
    w_pmpcfg3(new_cfg);
}


int main(void)
{
    /* Init PMP */
    w_pmpcfg0(0x0);
    w_pmpcfg1(0x0);
    w_pmpcfg2(0x0);
    w_pmpcfg3(0x0);

    /* Init ktables */
    thread_init_ktable();
    user_irq_init_ktable();
    as_t_init_ktable();
    ktimer_init_ktable();
    fpage_table_init_ktable();

    /* Init subsystems */
    sched_init();
    syscall_init();
    ktimer_event_init();
    memory_init();
    thread_init_subsys();

    UART_init(115200, 0);
    UART_receive_intr_matr_init();
    dbg_init(DL_BASIC | DL_KDB  | DL_KTABLE | DL_SOFTIRQ | DL_THREAD |
             DL_KTIMER | DL_SYSCALL | DL_SCHEDULE | DL_MEMORY | DL_IPC);
    __l4_printf("%s", banner);

    ktimer_event_create(64, ipc_deliver, (ktimer_event_t *) NULL);

    create_idle_thread();
    create_root_thread();
    create_kernel_thread();

    current = get_kernel_thread();

    user_irq_init();
    irq_init();
    map_intr_vector();
    system_timer_init();

    switch_to_kernel();

    /* Not reached */
    return 0;
}

extern void __l4_start(void)
{

    /* Fill bss with zeroes */
    for (char *p = (char *)&bss_start; p < (char *)&bss_end; p++) {
        *p = 0;
    }

    /* entry point */
    main();
}
