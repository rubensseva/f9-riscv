/* Copyright (c) 2013 The F9 Microkernel Project. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <irq.h>
#include <softirq.h>
#include <thread.h>
#include <ktimer.h>
#include <sched.h>
#include <platform/riscv.h>
#include <platform/link.h>
#include <root_thread.h>

/*
 * @file systhread.c
 * @brief Three main system threads: kernel, root and idle
 * */

tcb_t *idle;
tcb_t *kernel;
tcb_t *root;

tcb_t *get_kernel_thread() {
	return kernel;
}

utcb_t root_utcb __KIP;

static void kernel_thread(void);
static void idle_thread(void);

void create_root_thread(void)
{
	root = thread_init(TID_TO_GLOBALID(THREAD_ROOT), &root_utcb);
	root->name = "root_thread";
	thread_space(root, TID_TO_GLOBALID(THREAD_ROOT), &root_utcb);
	as_map_user(root->as);

	uint32_t regs[4] = {
		[REG_A0] = (uint32_t) &kip,
		[REG_A1] = (uint32_t) root->utcb,
		[REG_A2] = 0,
		[REG_A3] = 0,
	};

	thread_init_ctx((void *) &root_stack_end, root_thread, regs, root);

	root->stack_base = (memptr_t) &root_stack_start;
	root->stack_size = (uint32_t) &root_stack_end -
	                   (uint32_t) &root_stack_start;

	sched_slot_dispatch(SSI_ROOT_THREAD, root);
	root->state = T_RUNNABLE;
}

void create_kernel_thread(void)
{
	kernel = thread_init(TID_TO_GLOBALID(THREAD_KERNEL), NULL);
	kernel->name = "kernel_thread";

	thread_init_kernel_ctx(&kernel_stack_end, kernel);

	/* This will prevent running other threads
	 * than kernel until it will be initialized */
	sched_slot_dispatch(SSI_SOFTIRQ, kernel);
	kernel->state = T_RUNNABLE;
}

void create_idle_thread(void)
{
	idle = thread_init(TID_TO_GLOBALID(THREAD_IDLE), NULL);
	idle->name = "idle_thread";
	thread_init_ctx((void *) &idle_stack_end, idle_thread, NULL, idle);

	sched_slot_dispatch(SSI_IDLE, idle);
	idle->state = T_RUNNABLE;
}

void switch_to_kernel(void)
{
	/* Set previous privilege level to M mode for kernel thread
	   (kernel thread runs in m-mode, other threads run in s-mode) */
	unsigned long x = r_mstatus();
	x &= ~MSTATUS_MPP_MASK;
	x |= MSTATUS_MPP_M;
	w_mstatus(x);
	init_ctx_switch(kernel->ctx.sp, kernel_thread);
}

void set_kernel_state(thread_state_t state)
{
	kernel->state = state;
}

static void kernel_thread(void)
{
	while (1) {
		/* If all softirqs processed, call SVC to
		 * switch context immediately */
		softirq_execute();
		__asm__ __volatile__ ("ecall");
	}
}

static void idle_thread(void)
{
	while (1) {
		wait_for_interrupt();
	}
}
