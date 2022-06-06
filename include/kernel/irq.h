/* Copyright (c) 2013 The F9 Microkernel Project. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef IRQ_H_
#define IRQ_H_

#include <softirq.h>
#include <thread.h>
#include <sched.h>
#include <platform/link.h>

void irq_init(void);


/* Do the initial context switch.
 * We simulate a context switch by manually setting the stack pointer and mepc */
#define init_ctx_switch(sp, pc)							\
	__asm__ __volatile__ ("mv sp, %0" : : "r" (sp));	\
    __asm__ __volatile__ ("csrw mepc, %0" : : "r" (pc));     \
	__asm__ __volatile__ ("mret");

#define context_switch(from, to)					\
	{								\
		thread_switch((to));					\
	}


#endif	/* IRQ_H_ */
