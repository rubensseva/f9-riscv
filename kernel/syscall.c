/* Copyright (c) 2013, 2014 The F9 Microkernel Project. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <syscall.h>
#include <softirq.h>
#include <thread.h>
#include <ipc.h>
#include <l4/utcb.h>
#include <memory.h>
#include <irq.h>
#include <debug.h>

__BSS tcb_t *caller;

void svc_handler(void)
{
	extern tcb_t *kernel;

	/* Kernel requests context switch, satisfy it */
	if (thread_current() == kernel)
		return;

	caller = thread_current();
	caller->state = T_SVC_BLOCKED;

	softirq_schedule(SYSCALL_SOFTIRQ);
}


void syscall_init()
{
	softirq_register(SYSCALL_SOFTIRQ, syscall_handler);
}

static void sys_thread_control(uint32_t *param1)
{
	l4_thread_t dest = param1[REG_A1];
	l4_thread_t space = param1[REG_A2];
	/* l4_thread_t scheduler = param1[REG_A3]; */
	l4_thread_t pager = param1[REG_A4];

	if (space != L4_NILTHREAD) {
		/* Creation of thread */
		void *utcb = (void*) param1[REG_A5];
		mempool_t *utcb_pool = mempool_getbyid(mempool_search((memptr_t) utcb,
		                                       UTCB_SIZE));

		if (!utcb_pool || !(utcb_pool->flags & (MP_UR | MP_UW))) {
			/* Incorrect UTCB relocation */
			return;
		}

		tcb_t *thr = thread_create(dest, utcb);
		thread_space(thr, space, utcb);
		thr->utcb->t_pager = pager;
		/* TODO: This needs another look
		   What reg to use? Whats this for? Set destination register to one.. no idea why? */
		param1[REG_A1] = 1;
	} else {
		/* Removal of thread */
		tcb_t *thr = thread_by_globalid(dest);
		thread_free_space(thr);
		thread_destroy(thr);
	}
}

void syscall_handler()
{
	uint32_t *sc_param1 = (uint32_t *) caller->ctx.sp;
	uint32_t sc_num = sc_param1[REG_A0];
	if (sc_num == SYS_THREAD_CONTROL) {
		/* Simply call thread_create
		 * TODO: checking globalid
		 * TODO: pagers and schedulers
		 */
		sys_thread_control(sc_param1);
		caller->state = T_RUNNABLE;
	} else if (sc_num == SYS_IPC) {
		sys_ipc(sc_param1);
	} else {
		dbg_printf(DL_SYSCALL,
		           "SVC: %d called [%d, %d, %d, %d]\n", sc_num,
		           sc_param1[REG_A0], sc_param1[REG_A1],
		           sc_param1[REG_A2], sc_param1[REG_A3]);
		caller->state = T_RUNNABLE;
	}
}
