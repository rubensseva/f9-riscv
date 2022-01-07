/* Copyright (c) 2013 The F9 Microkernel Project. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef L4_UTCB_H_
#define L4_UTCB_H_

#include <types.h>

struct utcb {
	l4_thread_t t_globalid;
	uint32_t	processor_no;
	uint32_t 	user_defined_handle;	/* NOT used by kernel */
	l4_thread_t	t_pager;

	uint32_t	exception_handler;
	uint32_t	flags;		/* COP/PREEMPT flags (not used) */
	uint32_t	xfer_timeouts;
	uint32_t	error_code;

	l4_thread_t	intended_receiver;
	l4_thread_t	sender;
	uint64_t	thread_word_1;
	uint64_t	thread_word_2;
	uint64_t	mr[8];		/* MRs 8-15 (0-8 are laying in
					   r4..r11 [thread's context]) */
	uint64_t	br[8];
	uint64_t	reserved[4];
};

typedef struct utcb utcb_t;

#define UTCB_SIZE		128

#endif	/* L4_UTCB_H_ */
