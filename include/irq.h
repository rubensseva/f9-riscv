/* Copyright (c) 2013 The F9 Microkernel Project. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef PLATFORM_IRQ_H_
#define PLATFORM_IRQ_H_

#include <softirq.h>
#include <thread.h>
#include <sched.h>
#include <platform/link.h>
// #include <platform/cortex_m.h>

void irq_init(void);


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
