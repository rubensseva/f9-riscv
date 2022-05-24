/* Copyright (c) 2013 The F9 Microkernel Project. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/*
 * Fpage chain functions
 */

#include <types.h>
#include <memory.h>
#include <fpage.h>
#include <fpage_impl.h>
#include <debug.h>
#include <config.h>

#include <error.h>

DECLARE_KTABLE(fpage_t, fpage_table, CONFIG_MAX_FPAGES);

void fpage_table_init_ktable() {
	ktable_init(&fpage_table, (ptr_t)kt_fpage_table_data);
}

/**
 * Insert chain of fpages into address space
 * @param first, last - fpage chain
 * @param as address space
 *
 * NOTE: Not checks if as and this chain overlapping
 */
static void insert_fpage_chain_to_as(as_t *as, fpage_t *first, fpage_t *last)
{
	fpage_t *fp = as->first;

	if (!fp) {
		/* First chain in AS */
		as->first = first;
		return;
	}

	if (last->fpage.base < fp->fpage.base) {
		/* Add chain into beginning */
		last->as_next = as->first;
		as->first = first;
	} else {
		/* Search for chain in the middle */
		while (fp->as_next) {
			if (FPAGE_BASE(last) < FPAGE_BASE(fp->as_next)) {
				last->as_next = fp->as_next;
				break;
			}
			fp = fp->as_next;
		}
		fp->as_next = first;
	}
}

/**
 * Insert single fpage into address space
 * @param fpage fpage
 * @param as address space
 *
 * NOTE: Not checks if as and this chain overlapping
 */
static void insert_fpage_to_as(as_t *as, fpage_t *fpage)
{
	insert_fpage_chain_to_as(as, fpage, fpage);
}

/**
 * Remove fpage from address space
 * @param fp fpage
 * @param as address space
 *
 * If fpage is not exist in as fpages list, does nothing
 */
static void remove_fpage_from_as(as_t *as, fpage_t *fp)
{
	remove_fpage_from_list(as, fp, first, as_next);
	remove_fpage_from_list(as, fp, mpu_first, mpu_next);
}

/* FIXME: Support for bit-bang regions. */
/**
 * Create and initialize new fpage
 * @param base base address of fpage
 * @param shift (1 << shift) - fpage size
 * @param mpid - id of mpool
 */
static fpage_t *create_fpage(memptr_t base, size_t size, int mpid)
{
	if (size > CONFIG_FPAGE_MAX_SIZE) {
		dbg_printf(DL_EMERG, "ERROR: fpage is too large. base: 0x%p, size: %d\n", base, size);
	}

	fpage_t *fpage = (fpage_t *) ktable_alloc(&fpage_table);

	assert((intptr_t) fpage);

	fpage->as_next = NULL;
	fpage->map_next = fpage; 	/* That is first fpage in mapping */
	fpage->mpu_next = NULL;
	fpage->fpage.mpid = mpid;
	fpage->fpage.flags = 0;
	fpage->fpage.rwx = MP_USER_PERM(mempool_getbyid(mpid)->flags);

	fpage->fpage.base = base;
	fpage->fpage.size = size;

	if (mempool_getbyid(mpid)->flags & MP_MAP_ALWAYS)
		fpage->fpage.flags |= FPAGE_ALWAYS;

	return fpage;
}

void destroy_fpage(fpage_t *fpage)
{
	ktable_free(&fpage_table, fpage);
}

static void create_fpage_chain(memptr_t base, size_t size, int mpid,
                               fpage_t **pfirst, fpage_t **plast)
{
	// Make sure the base address is four-byte aligned
	if ((base & ~0x3) != base) {
		dbg_printf(DL_EMERG, "ERROR: cant create fpage chaing since address is not four-byte aligned. base: 0x%p, size: %d\n", base, size);
		return;
	}
	if (size > 65535) {
		dbg_printf(DL_EMERG, "ERROR: fpage is too large. base: 0x%p, size: %d\n", base, size);
		return;
	}

	/* int shift, sshift, bshift; */
	fpage_t *fpage = NULL;
	fpage = create_fpage(base, size, mpid);
	*pfirst = fpage;
	*plast = fpage;
}

fpage_t *split_fpage(as_t *as, fpage_t *fpage, memptr_t split)
{
	memptr_t base = fpage->fpage.base,
	         end = fpage->fpage.base + fpage->fpage.size;
	/* fpage_t *lfirst = NULL, *llast = NULL, *rfirst = NULL, *rlast = NULL; */
	split = mempool_align(fpage->fpage.mpid, split);

	if (!as)
		return NULL;

	/* Check if we can split something */
	if (split == base || split == end) {
		return fpage;
	}

	/* Since map_next is a cyclic list, if fpage->map_next == fpage then it is "empty" */
	if (fpage->map_next != fpage) {
		/* Splitting not supported for mapped pages */
		/* UNIMPLIMENTED */
		dbg_printf(DL_MEMORY,
					"ERROR: Splitting not supported for mapped pages. as: 0x%p, fpage: 0x%p, split: 0x%p\n",
				   as, fpage, split);
		return NULL;
	}

	fpage_t *first = create_fpage(base, (split - base),
	                   fpage->fpage.mpid);
	fpage_t *last = create_fpage(split, (end - split),
	                   fpage->fpage.mpid);

	remove_fpage_from_as(as, fpage);
	ktable_free(&fpage_table, fpage);

	first->as_next = last;
	insert_fpage_chain_to_as(as, first, last);

	return first;
}


int assign_fpages_ext(int mpid, as_t *as, memptr_t base, size_t size,
                      fpage_t **pfirst, fpage_t **plast)
{
	fpage_t **fp;
	memptr_t  end;

	if (size <= 0)
		return -1;

	/* if mpid is unknown, search using base addr */
	if (mpid == -1) {
		if ((mpid = mempool_search(base, size)) == -1) {
			/* Cannot find appropriate mempool, return error */
			dbg_printf(DL_MEMORY,
					   "ERROR: Cannot find appropriate mempool. mpid: %d, base: 0x%p, size: %d\n",
					   mpid, base, size);
			return -1;
		}
	}

	end = base + size;

	if (as) {
		/* find unmapped space */
		fp = &as->first;
		while (base < end && *fp) {
			if (base < FPAGE_BASE(*fp)) {
				fpage_t *first = NULL, *last = NULL;
				size = (end < FPAGE_BASE(*fp) ? end : FPAGE_BASE(*fp)) - base;

				dbg_printf(DL_MEMORY,
				           "MEM: fpage chain %s [b:%p, sz:%p] as %p\n",
				           mempool_getbyid(mpid)->name, base, size, as);

				create_fpage_chain(mempool_align(mpid, base),
				                   mempool_align(mpid, size),
				                   mpid, &first, &last);

				last->as_next = *fp;
				*fp = first;
				fp = &last->as_next;

				if (!*pfirst)
					*pfirst = first;
				*plast = last;

				base = FPAGE_END(*fp);
			} else if (base < FPAGE_END(*fp)) {
				if (!*pfirst)
					*pfirst = *fp;
				*plast = *fp;

				base = FPAGE_END(*fp);
			}

			fp = &(*fp)->as_next;
		}

		if (base < end) {
			fpage_t *first = NULL, *last = NULL;
			size = end - base;

			dbg_printf(DL_MEMORY,
			           "MEM: fpage chain %s [b:%p, sz:%p] as %p\n",
			           mempool_getbyid(mpid)->name, base, size, as);

			create_fpage_chain(mempool_align(mpid, base),
			                   mempool_align(mpid, size),
			                   mpid, &first, &last);

			*fp = first;

			if (!*pfirst)
				*pfirst = first;
			*plast = last;
		}
	} else {
		dbg_printf(DL_MEMORY,
		           "MEM: fpage chain %s [b:%p, sz:%p] as %p\n",
		           mempool_getbyid(mpid)->name, base, size, as);

		create_fpage_chain(mempool_align(mpid, base),
		                   mempool_align(mpid, size),
		                   mpid, pfirst, plast);
	}

	return 0;
}

int assign_fpages(as_t *as, memptr_t base, size_t size)
{
	fpage_t *first = NULL, *last = NULL;
	return assign_fpages_ext(-1, as, base, size, &first, &last);
}


int map_fpage(as_t *src, as_t *dst, fpage_t *fpage, map_action_t action)
{
	fpage_t *fpmap = (fpage_t *) ktable_alloc(&fpage_table);

	/* FIXME: check for fpmap == NULL */
	fpmap->as_next = NULL;
	fpmap->mpu_next = NULL;

	/* Copy fpage description */
	fpmap->raw[0] = fpage->raw[0];
	fpmap->raw[1] = fpage->raw[1];

	/* Set flags correctly */
	if (action == MAP)
		fpage->fpage.flags |= FPAGE_MAPPED;
	fpmap->fpage.flags = FPAGE_CLONE;

	/* Insert into mapee list */
	fpmap->map_next = fpage->map_next;
	fpage->map_next = fpmap;

	/* Insert into AS */
	insert_fpage_to_as(dst, fpmap);

	dbg_printf(DL_MEMORY, "MEM: %s fpage 0x%p (base 0x%x) from 0x%p to 0x%p\n",
	           (action == MAP) ? "mapped" : "granted", fpage, fpage->fpage.base, src, dst);

	return 0;
}

int unmap_fpage(as_t *as, fpage_t *fpage)
{
	fpage_t *fpprev = fpage;

	dbg_printf(DL_MEMORY, "MEM: unmapped fpage %p from %p\n", fpage, as);

	/* Fpages that are not mapped or granted
	 * are destroyed with its AS
	 */
	if (!(fpage->fpage.flags & FPAGE_CLONE))
		return -1;

	while (fpprev->map_next != fpage)
		fpprev = fpprev->map_next;

	/* Clear flags */
	fpprev->fpage.flags &= ~FPAGE_MAPPED;

	fpprev->map_next = fpage->map_next;
	remove_fpage_from_as(as, fpage);

	ktable_free(&fpage_table, fpage);

	return 0;
}
