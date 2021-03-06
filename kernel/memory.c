/* Copyright (c) 2013 The F9 Microkernel Project. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <memory.h>
#include <error.h>
#include <thread.h>
#include <lib/ktable.h>
#include <fpage_impl.h>
#include <kip.h>
#include <debug.h>

/**
 * @file    memory.c
 * @brief   Memory management subsystem.
 *
 * Unlike traditional L4 kernels, built for "large systems", we focus on
 * small MCU with energy efficiency.  Therefore,
 *   - We don't have virtual memory and pages
 *   - RAM is small, but physical address space is relatively large (32-bit),
 *     which includes devices, flash, bit-band regions
 *   - We have memory protection unit with only 8 regions
 *
 * Memory management is split into three conceptions:
 *   - Memory pool (mempool_t), which represent area of PAS with specific
 *     attributes (hardcoded in memmap table),
 *   - Flexible page (fpage_t) - unlike traditional fpages in L4, they
 *     represent MPU region instead
 *   - Address space (as_t) - sorted list of fpages bound to specific thread(s)
 *
 * MPU in Cortex-M supports regions of 2^n size only, so if we want to create
 * page of 96 bytes for example, we should split it into smaller ones, and
 * create fpage chain consisting of 32 byte and 64 byte fpages.
 * That is exactly the reason why the implementation looks complicated.
 *
 * Obvious way is to use regions of standard size (e.g. 128 bytes), but it is
 * very wasteful in terms of memory faults (we have only 8 regions for MPU,
 * so for large ASes, we will often receive memmanage faults), and fpage table.
 */

/**
 * Memory map of MPU.
 * Translated into memdesc array in KIP by memory_init
 */
static mempool_t memmap[] = {
	DECLARE_MEMPOOL_2("KTEXT", kernel_text,
		MP_KR | MP_KX | MP_NO_FPAGE, MPT_KERNEL_TEXT),
	DECLARE_MEMPOOL_2("UTEXT", user_text,
		MP_UR | MP_UX | MP_MEMPOOL | MP_MAP_ALWAYS, MPT_USER_TEXT),
	DECLARE_MEMPOOL_2("KIP", kip,
		MP_KR | MP_KW | MP_UR | MP_SRAM, MPT_KERNEL_DATA),
	DECLARE_MEMPOOL("KDATA", &kip_end, &kernel_data_end,
		MP_KR | MP_KW | MP_NO_FPAGE, MPT_KERNEL_DATA),
	DECLARE_MEMPOOL_2("KBSS",  kernel_bss,
		MP_KR | MP_KW | MP_NO_FPAGE, MPT_KERNEL_DATA),
	DECLARE_MEMPOOL_2("UDATA", user_data,
		MP_UR | MP_UW | MP_MEMPOOL | MP_MAP_ALWAYS, MPT_USER_DATA),
	DECLARE_MEMPOOL_2("UBSS",  user_bss,
		MP_UR | MP_UW | MP_MEMPOOL  | MP_MAP_ALWAYS, MPT_USER_DATA),
	DECLARE_MEMPOOL_2("MEM0",  mem0,
					  MP_UR | MP_UW | MP_SRAM, MPT_AVAILABLE),
	DECLARE_MEMPOOL("PERI_UART0",  CONFIG_PERIPHERAL_MEM_UART0_START, CONFIG_PERIPHERAL_MEM_UART0_START + CONFIG_PERIPHERAL_MEM_UART0_SIZE,
					MP_KR | MP_KW | MP_UR | MP_UW | MP_DEVICES, MPT_DEVICES),
	DECLARE_MEMPOOL("PERI_TIMERG0",  CONFIG_PERIPHERAL_MEM_TIMERG0_START, CONFIG_PERIPHERAL_MEM_TIMERG0_START + CONFIG_PERIPHERAL_MEM_TIMERG0_SIZE,
					MP_KR | MP_KW | MP_UR | MP_UW | MP_DEVICES, MPT_DEVICES),
};

DECLARE_KTABLE(as_t, as_table, CONFIG_MAX_ADRESS_SPACES);

void as_t_init_ktable()
{
	ktable_init(&as_table, (ptr_t) kt_as_table_data);
}

extern kip_mem_desc_t *mem_desc;
extern char *kip_extra;

/* Some helper functions */
/* size value must be 2^k */
static memptr_t addr_align(memptr_t addr, size_t size)
{
	return (addr + (size - 1)) & ~(size - 1);
}

#define CONFIG_SMALLEST_FPAGE_SIZE	4

memptr_t mempool_align(int mpid, memptr_t addr)
{
	if (memmap[mpid].flags & MP_FPAGE_MASK)
		return addr_align(addr, CONFIG_SMALLEST_FPAGE_SIZE);

	return INVALID_FPAGE_REGION;
}

int mempool_search(memptr_t base, size_t size)
{
	for (int i = 0; i < sizeof(memmap) / sizeof(mempool_t); ++i) {
		if ((memmap[i].start <= base) &&
		    (memmap[i].end >= (base + size))) {
			return i;
		}
	}
	return -1;
}

mempool_t *mempool_getbyid(int mpid)
{
	if (mpid == -1)
		return NULL;

	return memmap + mpid;
}

void memory_init()
{
	int j = 0;

	mem_desc = (kip_mem_desc_t *) kip_extra;

	/* Initialize mempool table in KIP */
	for (int i = 0; i < sizeof(memmap) / sizeof(mempool_t); ++i) {
		switch (memmap[i].tag) {
		case MPT_USER_DATA:
		case MPT_USER_TEXT:
		case MPT_DEVICES:
		case MPT_AVAILABLE:
			// Last 6 bits of base contains poolid
			mem_desc[j].base = addr_align(
					(memmap[i].start),
			                CONFIG_SMALLEST_FPAGE_SIZE) | i;
			// last 6 bits of size contains tag
			mem_desc[j].size = addr_align(
					(memmap[i].end - memmap[i].start),
					CONFIG_SMALLEST_FPAGE_SIZE) | memmap[i].tag;
			j++;
			break;
		}
	}

	kip.memory_info.s.memory_desc_ptr =
	    ((void *) mem_desc) - ((void *) &kip);
	kip.memory_info.s.n = j;

}

// INIT_HOOK(memory_init, INIT_LEVEL_KERNEL_EARLY);

/*
 * AS functions
 */

void as_setup_mpu(as_t *as, memptr_t sp, memptr_t pc,
                  memptr_t stack_base, size_t stack_size)
{
	fpage_t *mpu[8] = { NULL };
	fpage_t *fp;
	int mpu_first_i; // The first available entry in mpu[] after stack fpages are inserted
	int i, j;

	fpage_t *mpu_stack_first = NULL;

	/* Iterator for walking through stack */
	memptr_t start = stack_base;
	memptr_t end = stack_base + stack_size;

	/* Find stack fpages */
	fp = as->first;
	i = 0;
	while (i < 8 && fp && start < end) {
		if (addr_in_fpage(start, fp, 0)) {
			if (!mpu_stack_first)
				mpu_stack_first = fp;

			mpu[i++] = fp;
			start = FPAGE_END(fp);
		}
		fp = fp->as_next;
	}

	as->mpu_stack_first = mpu_stack_first;
	mpu_first_i = i; // Save the place in mpu[] after stack fpages have been inserted

	/*
	 * We walk through fpage list
	 * mpu_fp[0] are pc
	 * mpu_fp[1] are always-mapped fpages
	 * mpu_fp[2] are others
	 *
	 * What we do here is setup three linked lists of fpages that are meant to be setup by MPU.
	 * For each fpage in the address space, we check what "type" it is. Is it overlapping the PC,
	 * should it be always mapped, or all other fpages. For each of these categories, we build
	 * a list of ->mpu_next entries, linking them together.

	 * When we are done building the three different linked lists, we chain them together.
	 * Fpages overlapping the PC goes first, then the alwyas mapped fpages, then all the others.
	 */
	fp = as->mpu_first;
	if (!fp) {
		fpage_t *mpu_first[3] = {NULL};
		fpage_t *mpu_fp[3] = {NULL};

		fp = as->first;
		/* Build three linked lists of ->mpu_next entries */
		while (fp) {
			int priv = 2;

			if (addr_in_fpage(pc, fp, 0)) {
				priv = 0;
			} else if (fp->fpage.flags & FPAGE_ALWAYS) {
				priv = 1;
			}

			if (!mpu_first[priv]) {
				mpu_first[priv] = fp;
				mpu_fp[priv] = fp;
			} else {
				mpu_fp[priv]->mpu_next = fp;
				mpu_fp[priv] = fp;
			}

			fp = fp->as_next;
		}

		/* Chain together the three different linked lists to one linked list */
		if (mpu_first[1]) {
			mpu_fp[1]->mpu_next = mpu_first[2];
		} else {
			mpu_first[1] = mpu_first[2];
		}
		if (mpu_first[0]) {
			mpu_fp[0]->mpu_next = mpu_first[1];
		} else {
			mpu_first[0] = mpu_first[1];
		}
		/* Set the ->mpu_first of the address space to the final linked list, beginning with
		   fpages overlapping with the PC */
		as->mpu_first = mpu_first[0];
	}

	/* Prevent link to stack pages

	 * Here, we walk through the ->mpu_first linked list (which was either newly created, or
	 * persisted from a previous run of this function with the same address space). We check
	 * if the fpage is a stack fpage, in which case it should already have been added to mpu[]
	 * with an index lower than mpu_first_i. As long the fpage is NOT a stack fpage, then we
	 * add it to the mpu[] array, with an index AFTER mpu_first_i.
	 */
	for (fp = as->mpu_first; i < 8 && fp; fp = fp->mpu_next) {
		for (j = 0; j < mpu_first_i; j++) {
			if (fp == mpu[j]) {
				break;
			}
		}
		if (j == mpu_first_i) {
			mpu[i++] = fp;
		}
	}

	as->mpu_first = mpu[mpu_first_i];

	/* Setup MPU stack regions */
	for (j = 0; j < mpu_first_i; ++j) {
		mpu_setup_region(j, mpu[j]);

		if (j < mpu_first_i - 1)
			mpu[j]->mpu_next = mpu[j + 1];
		else
			mpu[j]->mpu_next = NULL;
	}

	/* Setup MPU fifo regions */
	for (; j < i; ++j) {
		mpu_setup_region(j, mpu[j]);

		if (j < i - 1)
			mpu[j]->mpu_next = mpu[j + 1];
	}

	/* Clean unused MPU regions */
	for (; j < 8; ++j) {
		mpu_setup_region(j, NULL);
	}
}

void as_map_user(as_t *as)
{
	for (int i = 0; i < sizeof(memmap) / sizeof(mempool_t); ++i) {
		switch (memmap[i].tag) {
		case MPT_USER_DATA:
		case MPT_USER_TEXT:
		case MPT_DEVICES:
			/* Map user text, data and hardware device memory */
			assign_fpages(as, memmap[i].start,
			              (memmap[i].end - memmap[i].start));
		}
	}
}

void as_map_ktext(as_t *as)
{
	for (int i = 0; i < sizeof(memmap) / sizeof(mempool_t); ++i) {
		if (memmap[i].tag == MPT_KERNEL_TEXT) {
			assign_fpages(as, memmap[i].start,
			              (memmap[i].end - memmap[i].start));
		}
	}
}

as_t *as_create(uint32_t as_spaceid)
{
	as_t *as = (as_t *) ktable_alloc(&as_table);

	/* assert as == NULL */

	as->as_spaceid = as_spaceid;
	as->first = NULL;
	as->shared = 0;

	return as;
}

void as_destroy(as_t *as)
{
	fpage_t *fp, *fpnext;
	fp = as->first;

	if (as->shared > 0) {
		--as->shared;
		return;
	}

	/*
	 * FIXME: What if a CLONED fpage which is MAPPED is to be deleted
	 */
	while (fp) {
		if (fp->fpage.flags & FPAGE_CLONE)
			unmap_fpage(as, fp);
		else
			destroy_fpage(fp);

		fpnext = fp->as_next;
		fp = fpnext;
	}

	ktable_free(&as_table, (void *) as);
}

int map_area(as_t *src, as_t *dst, memptr_t base, size_t size,
             map_action_t action, int is_priviliged)
{
	/* Most complicated part of mapping subsystem */
	memptr_t end = base + size, probe = base;
	fpage_t *fp = src->first, *first = NULL, *last = NULL;
	int first_last_equal = 0;

	/* FIXME: reverse mappings (i.e. thread 1 maps 0x1000 to thread 2,
	 * than thread 2 does the same to thread 1).
	 */

	/* For priviliged thread (ROOT), we use shadowed mapping,
	 * so first we will check if that fpages exist and then
	 * create them.
	 */

	/* FIXME: checking existence of fpages */

	if (is_priviliged) {
		assign_fpages_ext(-1, src, base, size, &first, &last);
		if (src == dst) {
			/* Maps to itself, ignore other actions */
			return 0;
		}
	} else {
		if (src == dst) {
			/* Maps to itself, ignore other actions */
			return 0;
		}
		/* We should determine first and last fpage we will map to:
		 *
		 * +----------+    +----------+    +----------+
		 * |   first  | -> |          | -> |  last    |
		 * +----------+    +----------+    +----------+
		 *     ^base            ^    +size      =    ^end
		 *                      | probe
		 *
		 * probe checks that addresses in fpage are sequental
		 */
		while (fp) {
			if (!first && addr_in_fpage(base, fp, 0)) {
				first = fp;
			}

			if (!last && addr_in_fpage(end, fp, 1)) {
				last = fp;
				break;
			}

			if (first) {
				/* Check weather if addresses in fpage list
				 * are sequental */
				if (!addr_in_fpage(probe, fp, 1)) {
					dbg_printf(DL_EMERG,
							"ERROR: Addresses in fpage list not sequential");
					return -1;
				}

				probe += FPAGE_SIZE(fp);
			}

			fp = fp->as_next;
		}
	}

	if (!last || !first) {
		dbg_printf(DL_EMERG,
		           "ERROR: Requested map area not in address space of mapper thread, or other error\n");
		return -1;
	}

	/* Since the mapped memory area doesnt have to follow fpage borders, we need to split them up */

	if (first == last)
		first_last_equal = 1;

	first = split_fpage(src, first, base);

	/* If we performed a split, then we need to grab the second fpage of the new pair of fpages from the split */
	if (first != last) {
		first = first->as_next;
	}

	/* If first was equal to last, then last fpage is invalidated at this point, so we update it */
	if (first_last_equal) {
		last = first;
	}

	last = split_fpage(src, last, end);
	/* If first was equal to last, then first fpage is invalidated at this point, so we update it */
	if (first_last_equal) {
		first = last;
	}


	if (!last || !first) {
		/* Splitting not supported for mapped pages */
		/* UNIMPLIMENTED */
		/* TODO: Why does this condition mean that we do not support splitting of mapped pages? */
		dbg_printf(DL_EMERG,
		           "ERROR: Splitting not supported for mapped pages\n");
		return -1;
	}

	/* Map chain of fpages */

	fp = first;
	while (fp != last) {
		map_fpage(src, dst, fp, action);
		fp = fp->as_next;
	}
	map_fpage(src, dst, fp, action);

	return 0;
}

#ifdef CONFIG_KDB

static char *kdb_mempool_prop(mempool_t *mp)
{
	static char mempool[10] = "--- --- -";
	/* mempool[0] = (mp->flags & MP_KR) ? 'r' : '-'; */
	/* mempool[1] = (mp->flags & MP_KW) ? 'w' : '-'; */
	/* mempool[2] = (mp->flags & MP_KX) ? 'x' : '-'; */

	/* mempool[4] = (mp->flags & MP_UR) ? 'r' : '-'; */
	/* mempool[5] = (mp->flags & MP_UW) ? 'w' : '-'; */
	/* mempool[6] = (mp->flags & MP_UX) ? 'x' : '-'; */

	/* mempool[8] = (mp->flags & MP_DEVICES) ? */
	/*              'D' : (mp->flags & MP_MEMPOOL) ? */
	/*              'M' : (mp->flags & MP_AHB_RAM) ? */
	/*              'A' : (mp->flags & MP_SRAM) ? */
	/*              'S' : 'N'; */
	return mempool;
}

void kdb_dump_mempool(void)
{
	dbg_printf(DL_KDB,
	           "%2s %20s %10s [%8s:%8s] %10s\n",
	           "ID", "NAME", "SIZE", "START", "END", "FLAGS");

	for (int i = 0; i < sizeof(memmap) / sizeof(mempool_t); ++i) {
		dbg_printf(DL_KDB,
		           "%2d %20s %10d [%p:%p] %10s\n",
		           i,
		           memmap[i].name, (memmap[i].end - memmap[i].start),
		           memmap[i].start, memmap[i].end,
		           kdb_mempool_prop(&(memmap[i])));
	}
}

void kdb_dump_as(void)
{
	enum {
		DBG_ASYNC, DBG_PANIC
	};
	/* dbg_state; */
	int idx = 0, nl = 0, i;
	as_t *as = NULL;
	fpage_t *fpage = NULL;

	for_each_in_ktable(as, idx, &as_table) {
		fpage = as->first;
		dbg_printf(DL_KDB, "Address Space %p\n", as->as_spaceid);

		while (fpage) {
			fpage->used = 0;
			fpage = fpage->as_next;
		}

		i = 0;
		fpage = as->mpu_stack_first;
		while (i < 8 && fpage) {
			fpage->used = 1;
			fpage = fpage->mpu_next;
			++i;
		}

		fpage = as->mpu_first;
		while (i < 8 && fpage) {
			fpage->used = 1;
			fpage = fpage->mpu_next;
			++i;
		}

		nl = 0;
		fpage = as->first;
		while (fpage) {
			dbg_printf(DL_KDB,
			           "MEM: %c fpage %5s [b:%p, sz:2**%d]\n",
			           fpage->used ? 'o' : ' ',
			           memmap[fpage->fpage.mpid].name,
			           fpage->fpage.base, FPAGE_SIZE(fpage));
			fpage = fpage->as_next;
			++nl;
		}
	}
}

#endif	/* CONFIG_KDB */
