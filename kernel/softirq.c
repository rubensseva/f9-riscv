/* Copyright (c) 2013 The F9 Microkernel Project. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <types.h>
#include <irq.h>
#include <bitops.h>
#include <softirq.h>
#include <systhread.h>
#include <platform/riscv.h>
#include <interrupt.h>
#include <debug.h>

static softirq_t softirq[NR_SOFTIRQ];

void softirq_register(softirq_type_t type, softirq_handler_t handler)
{
	softirq[type].handler = handler;
	softirq[type].schedule = 0;
}

void softirq_schedule(softirq_type_t type)
{
	/* TODO: Consider doing this atomically */
	softirq[type].schedule = 1;
	set_kernel_state(T_RUNNABLE);
}

static char *softirq_names[NR_SOFTIRQ] __attribute__((used)) = {
	"Kernel timer events",
	"Asynchronous events",
	"System calls",
};

int softirq_execute()
{
	uint32_t softirq_schedule = 0, executed = 0, prev_mstatus;
retry:
	for (int i = 0; i < NR_SOFTIRQ; ++i) {
		/* TODO: Consider doing this atomically */
		if (softirq[i].schedule != 0 && softirq[i].handler) {
			softirq[i].handler();

			executed = 1;
			/* TODO: Consider doing this atomically */
			softirq[i].schedule = 0;

			dbg_printf(DL_SOFTIRQ,
			           "SOFTIRQ: executing %s\n", softirq_names[i]);
		}
	}

	prev_mstatus = interrupt_disable();

	softirq_schedule = 0;
	for (int i = 0; i < NR_SOFTIRQ; ++i) {
		softirq_schedule |= softirq[i].schedule;
	}

	set_kernel_state((softirq_schedule) ? T_RUNNABLE : T_INACTIVE);

	interrupt_restore(prev_mstatus);

	if (softirq_schedule)
		goto retry;

	return executed;
}

