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

static void sys_thread_control(l4_thread_t dest, l4_thread_t space, l4_thread_t pager, void* utcb)
{
	/* l4_thread_t dest = param1[REG_T0]; */
	/* l4_thread_t space = param1[REG_T1]; */
	/* l4_thread_t pager = param1[REG_T3]; */

	if (space != L4_NILTHREAD) {
		/* Creation of thread */
		/* void *utcb = (void *) param2[0];	/\* R4 *\/ */
		mempool_t *utcb_pool = mempool_getbyid(mempool_search((memptr_t) utcb,
		                                       UTCB_SIZE));

		if (!utcb_pool || !(utcb_pool->flags & (MP_UR | MP_UW))) {
			/* Incorrect UTCB relocation */
			return;
		}

		tcb_t *thr = thread_create(dest, utcb);
		thread_space(thr, space, utcb);
		thr->utcb->t_pager = pager;
		// TODO: What does this commented out line do?
		// param1[REG_T0] = 1;
	} else {
		/* Removal of thread */
		tcb_t *thr = thread_by_globalid(dest);
		thread_free_space(thr);
		thread_destroy(thr);
	}
}

void syscall_handler()
{
	uint64_t* a_regs = caller->ctx.a_regs;
	uint64_t sc_num = a_regs[0];               // system call number

	if (sc_num == SYS_THREAD_CONTROL) {
        sys_thread_control(a_regs[1],          // dest
						   a_regs[2],          // space
						   a_regs[3],          // pager
						   (void*) a_regs[4]); // utcb
		caller->state = T_RUNNABLE;
	} else if (sc_num == SYS_IPC) {
		sys_ipc(caller->ctx.a_regs[1],         // to_tid
				caller->ctx.a_regs[2],         // from_tid
				caller->ctx.a_regs[3]);        // timeout
	} else {
		caller->state = T_RUNNABLE;
	}
}
