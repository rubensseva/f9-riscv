/* Copyright (c) 2013 The F9 Microkernel Project. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef THREAD_H_
#define THREAD_H_

#include <types.h>
#include <lib/ktable.h>
#include <memory.h>
#include <kip.h>
#include <config.h>

#include <l4/utcb.h>

/**
 * @file thread.h
 * @brief Thread dispatcher definitions
 *
 * Thread ID type is declared in @file types.h and called l4_thread_t
 *
 * For Global Thread ID only high 18 bits are used and lower are reserved,
 * so we call higher meaningful value TID and use GLOBALID_TO_TID and
 * TID_TO_GLOBALID macroses for convertion.
 *
 * Constants:
 *   - L4_NILTHREAD  - nilthread
 *   - L4_ANYLOCALTHREAD - anylocalthread
 *   - L4_ANYTHREAD  - anythread
 */

#define L4_NILTHREAD		0
#define L4_ANYLOCALTHREAD	0xFFFFFFC0
#define L4_ANYTHREAD		0xFFFFFFFF

#define GLOBALID_TO_TID(id)	(id >> 14)
#define TID_TO_GLOBALID(id)	(id << 14)

#define THREAD_BY_TID(id)	 thread_by_globalid(TID_TO_GLOBALID(id))

typedef enum {
	THREAD_IDLE,
	THREAD_KERNEL,
	THREAD_ROOT,
	THREAD_INTERRUPT,
	THREAD_IRQ_REQUEST,
	THREAD_LOG,
	THREAD_SYS	= 16,				/* Systembase */
	THREAD_USER	= CONFIG_INTR_THREAD_MAX	/* Userbase */
} thread_tag_t;

typedef enum {
	T_INACTIVE,
	T_RUNNABLE,
	T_SVC_BLOCKED,
	T_RECV_BLOCKED,
	T_SEND_BLOCKED
} thread_state_t;

/* Multiply these by 4 to get the byte offset of the stack pointer */
/* Caution: There is a duplicate of this in user space code. If you change this one, change the one in
   user space as well (and when actually saving registers on the stack) */
enum register_stack_t {
	REG_A0,
	REG_A1,
	REG_A2,
	REG_A3,
	REG_A4,
	REG_A5,
	REG_A6,
    REG_A7,
	REG_RA,
	REG_SP,
	REG_GP,
	REG_TP,
	REG_T0,
	REG_T1,
	REG_T2,
	REG_T3,
	REG_T4,
	REG_T5,
	REG_T6,
	REG_S0,
	REG_S1,
	REG_S2,
	REG_S3,
	REG_S4,
	REG_S5,
	REG_S6,
	REG_S7,
	REG_S8,
	REG_S9,
	REG_S10,
	REG_S11
};

typedef struct {
	uint32_t mepc;
	uint32_t sp;
} context_t;

/**
 * Thread control block
 *
 * TCB is a tree of threads, linked by t_sibling (siblings) and t_parent/t_child
 * Contains pointers to thread's UTCB (User TCB) and address space
 */
struct tcb {
	l4_thread_t t_globalid;
	l4_thread_t t_localid;

	char* name;

	thread_state_t state;

	memptr_t stack_base;
	size_t stack_size;

	context_t ctx;

	as_t *as;
	struct utcb *utcb;

	l4_thread_t ipc_from;

	struct tcb *t_sibling;
	struct tcb *t_parent;
	struct tcb *t_child;

	uint32_t timeout_event;
};
typedef struct tcb tcb_t;

void thread_init_subsys(void);

tcb_t *thread_by_globalid(l4_thread_t globalid);

void thread_init_ktable();
tcb_t *thread_init(l4_thread_t globalid, utcb_t *utcb);
tcb_t *thread_create(l4_thread_t globalid, utcb_t *utcb);
void thread_destroy(tcb_t *thr);
void thread_space(tcb_t *thr, l4_thread_t spaceid, utcb_t *utcb);
void thread_free_space(tcb_t *thr);
void thread_init_ctx(void *sp, void *pc, void *rx, tcb_t *thr);
void thread_init_kernel_ctx(void *sp, tcb_t *thr);
void thread_switch(tcb_t *thr);

int thread_ispriviliged(tcb_t *thr);
int thread_isrunnable(tcb_t *thr);
tcb_t *thread_current(void);

int schedule(void);

void dump_threads(void);
void dump_current_thread(void);

#endif /* THREAD_H_ */
