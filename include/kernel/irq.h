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
 * We simulate a context switch by manually setting the stack pointer and mepc */
#define init_ctx_switch(sp, pc)							\
	__asm__ __volatile__ ("mv sp, %0" : : "r" (sp));	\
    __asm__ __volatile__ ("csrw mepc, %0" : : "r" (pc));     \
	__asm__ __volatile__ ("mret");

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
