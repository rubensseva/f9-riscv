/* Copyright (c) 2013 The F9 Microkernel Project. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef LIB_BITMAP_H_
#define LIB_BITMAP_H_

#include <link.h>
#include <bitops.h>
#include <types.h>

/**
 * @file 	bitmap.h
 * @brief 	Kernel bitmaps
 *
 * UPDATE FOR RISC-V port: no longer using bit band (obviously)
 * TODO: Update this comment properly
 *
 * Bit band bitmaps relocated in AHB SRAM, so we use BitBang addresses
 * accessing bits. Also uses linker segment ".bitmap" from platform/link.h
 *
 * Bitmap iterates using bitmap cursor (type bitmap_cursor_t).
 * For bit-band bitmaps it an address in bit-band region, for classic bitmaps,
 * it is simply number of bit.
 */

#define BITMAP_ALIGN	32
#define DECLARE_BITMAP(name, size) \
	static __BITMAP uint32_t name [ALIGNED(size, BITMAP_ALIGN)];

typedef uint32_t *bitmap_ptr_t;


typedef struct {
	bitmap_ptr_t bc_bitmap;
	int bc_bit;
} bitmap_cursor_t;

#define bitmap_cursor(bitmap, bit) \
	(bitmap_cursor_t) { .bc_bitmap = bitmap, .bc_bit = bit }
#define bitmap_cursor_id(cursor) \
	cursor.bc_bit
#define bitmap_cursor_goto_next(cursor) \
	cursor.bc_bit++

#define BITOFF(bit) \
	(bit % BITMAP_ALIGN)		/* bit offset inside 32-bit word */
#define BITMASK(bit) \
	(1 << BITOFF(bit))		/* Mask used for bit number N */
#define BITINDEX(bit) \
	(bit / BITMAP_ALIGN)		/* Bit index in bitmap array */
#define BITWORD(cursor) \
	cursor.bc_bitmap[BITINDEX(cursor.bc_bit)]

static inline void bitmap_set_bit(bitmap_cursor_t cursor)
{
	BITWORD(cursor) |= BITMASK(cursor.bc_bit);
}

static inline void bitmap_clear_bit(bitmap_cursor_t cursor)
{
	BITWORD(cursor) &= ~BITMASK(cursor.bc_bit);
}

static inline int bitmap_get_bit(bitmap_cursor_t cursor)
{
	return (BITWORD(cursor) >> BITOFF(cursor.bc_bit)) & 0x1;
}

static inline int bitmap_test_and_set_bit(bitmap_cursor_t cursor)
{
	// We send in the address of BITWORD(cursor) because we want to change that value
	return test_and_set_bit(&BITWORD(cursor), BITMASK(cursor.bc_bit));
}

#define for_each_in_bitmap(cursor, bitmap, size, start) \
	for (cursor = bitmap_cursor(bitmap, start); \
	     bitmap_cursor_id(cursor) < size;	\
	     bitmap_cursor_goto_next(cursor))

#endif /* LIB_BITMAP_H_ */
