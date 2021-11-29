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
// #include <platform/armv7m.h>
#include <irq.h>
#include <init_hook.h>

tcb_t *caller;

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

INIT_HOOK(syscall_init, INIT_LEVEL_KERNEL);

static void sys_thread_control(uint64_t *param1, uint64_t *param2)
{
	l4_thread_t dest = param1[REG_T0];
	l4_thread_t space = param1[REG_T1];
	l4_thread_t pager = param1[REG_T3];

	if (space != L4_NILTHREAD) {
		/* Creation of thread */
		void *utcb = (void *) param2[0];	/* R4 */
		mempool_t *utcb_pool = mempool_getbyid(mempool_search((memptr_t) utcb,
		                                       UTCB_SIZE));

		if (!utcb_pool || !(utcb_pool->flags & (MP_UR | MP_UW))) {
			/* Incorrect UTCB relocation */
			return;
		}

		tcb_t *thr = thread_create(dest, utcb);
		thread_space(thr, space, utcb);
		thr->utcb->t_pager = pager;
		// TODO: This needs another look
		// What reg to use? Whats this for?
		param1[REG_A5] = 1;
	} else {
		/* Removal of thread */
		tcb_t *thr = thread_by_globalid(dest);
		thread_free_space(thr);
		thread_destroy(thr);
	}
}

void syscall_handler()
{
	uint64_t *sc_param1 = (uint64_t *) caller->ctx.sp;
	uint64_t sc_num = sc_param1[REG_A0];
	uint64_t *sc_param2 = caller->ctx.a_regs;

	if (sc_num == SYS_THREAD_CONTROL) {
		/* Simply call thread_create
		 * TODO: checking globalid
		 * TODO: pagers and schedulers
		 */
		sys_thread_control(sc_param1, sc_param2);
		caller->state = T_RUNNABLE;
	} else if (sc_num == SYS_IPC) {
		sys_ipc(sc_param1);
	} else {
		/* dbg_printf(DL_SYSCALL, */
		/*            "SVC: %d called [%d, %d, %d, %d]\n", svc_num, */
		/*            svc_param1[REG_R0], svc_param1[REG_R1], */
		/*            svc_param1[REG_R2], svc_param1[REG_R3]); */
		caller->state = T_RUNNABLE;
	}
}
