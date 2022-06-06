/* Copyright (c) 2013, 2014 The F9 Microkernel Project. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <error.h>
#include <types.h>
#include <platform/riscv.h>
#include <ipc.h>
#include <sched.h>
#include <fpage_impl.h>
#include <ktimer.h>
#include <softirq.h>
#include <syscall.h>
#include <systhread.h>
#include <lib/string.h>
#include <config.h>
#include <thread.h>
#include <interrupt.h>
#include <platform/ESP32_C3_UART.h>
#include <platform/ESP32_C3.h>
#include <utility.h>
#include <stdio.h>
#include <debug.h>
#include <platform/system_timer.h>
#include <platform/mpu.h>
#include <platform/interrupt_matrix.h>

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

void cpu_freq_init() {
    /* Set CPU frequency to 160MHz (max) by setting all three config
       registers to 1 (see 6.2.4.1 in ESP32-C3 TRM) */

    volatile uint32_t *system_cpu_per_conf = REG(SYSTEM_REGISTERS_BASE + SYSTEM_CPU_PER_CONF_REG);

    /* Set SYSTEM_CPUPERIOD_SEL to 1 */
    *system_cpu_per_conf &= ~3;
    *system_cpu_per_conf |= 1;

    /* Set SYSTEM_PLL_FREQ_SEL to 1 */
    *system_cpu_per_conf |= (1 << 2);

    volatile uint32_t *system_sysclk_conf = REG(SYSTEM_REGISTERS_BASE + SYSTEM_SYSCLK_CONF_REG);
    /* Set SYSTEM_SOC_CLK_SEL to 1 */
    *system_sysclk_conf &= ~(3 << 10);
    *system_sysclk_conf |= (1 << 10);
}

int main(void)
{
    /* Set cpu frequency */
    cpu_freq_init();

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
    intr_setup(CONFIG_UART_CPU_INTR, 11);

    dbg_init(DL_BASIC | DL_KDB | DL_EMERG);
    /* dbg_init(DL_BASIC | DL_KDB  | DL_KTABLE | DL_SOFTIRQ | DL_THREAD | */
    /*          DL_KTIMER | DL_SYSCALL | DL_SCHEDULE | DL_MEMORY | DL_IPC); */
    /* dbg_init(DL_BASIC | DL_KDB  | DL_KTABLE | DL_THREAD | */
    /*          DL_KTIMER | DL_SYSCALL | DL_SCHEDULE | DL_MEMORY); */
    /* dbg_init(DL_BASIC | DL_KDB  | DL_KTABLE | DL_THREAD | */
    /*          DL_KTIMER | DL_SCHEDULE); */
    __l4_printf("%s", banner);

    ktimer_event_create(64, ipc_deliver, (ktimer_event_t *) NULL);

    create_idle_thread();
    create_root_thread();
    create_kernel_thread();

    current = get_kernel_thread();

    user_irq_init();
    irq_init();
    map_intr_vector((uint32_t)_vector_table);
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
