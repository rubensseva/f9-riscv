/* Copyright (c) 2013 The F9 Microkernel Project. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef PLATFORM_IRQ_H_
#define PLATFORM_IRQ_H_

#include <softirq.h>
#include <thread.h>
#include <sched.h>
#include <link.h>

void irq_init(void);


/* Do the initial context switch.
 * A couple of important things to note here:
 * 1. We simulate a context switch by manually setting the stack pointer and mepc
 * 2. We wait for the next-to-last instruction to enable interrupts, so that
 *    this "fake" context switch does not get interrupted and a "real" context
 *    switch is attempted
 * 3. Enabling of interrupts is done by setting the MIE bit in mstatus, by orring
 *    mstatus with 3.
 *
 * I'm not really sure how safe it is to just delay enabling interrupts to the end,
 * is it not possible that an interrupt can occur between enabling interrupts and
 * the mret instruction? It seems to work for now, but this should be kept in mind
 * in case some suspicious error appears in the initialization code */
#define init_ctx_switch(sp, pc)							\
	__asm__ __volatile__ ("mv sp, %0" : : "r" (sp));	\
    __asm__ __volatile__ ("csrw mepc, %0" : : "r" (pc));     \
	__asm__ __volatile__ (    \
        "csrr t0, mstatus         \n\t\
        ori t0, t0, 3             \n\t\
        csrw mstatus, t0          \n\t\
        mret                      \n\t");

#define irq_ecall() \
	__asm__ __volatile__ ("ecall");

#define irq_save(ctx) \
	__asm__ __volatile__ ("")

#define context_switch(from, to)					\
	{								\
		thread_switch((to));					\
	}

#define schedule_in_irq()						\
	{								\
		register tcb_t *sel;					\
		sel = schedule_select();				\
		if (sel != current)					\
			context_switch(current, sel);			\
	}

#define request_schedule()						\
	do {  } while (0)


#define NO_PREEMPTED_IRQ						\
	(*SCB_ICSR & SCB_ICSR_RETTOBASE)



extern volatile tcb_t *current;

#endif	/* PLATFORM_IRQ_H_ */
