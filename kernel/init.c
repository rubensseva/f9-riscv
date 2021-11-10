/* Copyright (c) 2013 The F9 Microkernel Project. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <error.h>

#include <syscall.h>
#include <ktimer.h>
#include <init_hook.h>

extern void __l4_start(void);
/* extern void memmanage_handler(void); */
/* extern void debugmon_handler(void); */
/* extern void pendsv_handler(void); */


#define MAX(a, b)			\
	((a) > (b) ? (a) : (b))

extern const init_struct init_hook_start[];
extern const init_struct init_hook_end[];
static unsigned int last_level = 0;

int run_init_hook(unsigned int level)
{
	unsigned int max_called_level = last_level;

	for (const init_struct *ptr = init_hook_start;
	     ptr != init_hook_end; ++ptr)
		if ((ptr->level > last_level) &&
		    (ptr->level <= level)) {
			max_called_level = MAX(max_called_level, ptr->level);
			ptr->hook();
		}

	last_level = max_called_level;

	return last_level;
}
