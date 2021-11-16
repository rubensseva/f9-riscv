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
// #include <platform/cortex_m.h>

void irq_init(void);


/* Initial context switches to kernel.
 * It simulates interrupt to save corect context on stack.
 * When interrupts are enabled, it will jump to interrupt handler
 * and then return to normal execution of kernel code.
 *
 * // msr (general purpose register to psr (program status register))
 * // msp (main stack pointer)
 * // so msr msp, r0 means move r0 into master stack pointer (i think)
 *
 * CPS (Change Processor State)
 * CPSIE i  ; Enable interrupts and configurable fault handlers (clear PRIMASK)
 * bx jump, with hint that this is not a function return
 *
 * old:
	__asm__ __volatile__ ("mov sp, %0" : : "r" ((ctx)->sp));	\
	__asm__ __volatile__ ("msr msp, r0");				\
	__asm__ __volatile__ ("mov r1, %0" : : "r" (pc));		\
	__asm__ __volatile__ ("cpsie i");				\
	__asm__ __volatile__ ("bx r1");
	// jalr x0, rs, 0 - Jump register
	// jalr x0, x1, 0 - Return from subroutine
 */
#define init_ctx_switch(ctx, pc) \
	__asm__ __volatile__ ("add sp, x0, %0" : : "r" ((ctx)->sp));	\
	__asm__ __volatile__ ("jalr x0, %0, 0" : : "r" (pc));		\


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
