/* Copyright (c) 2013 The F9 Microkernel Project. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stdint.h>

/** This function was previously implement with inline assembly. To keep things
	simple, its implemented in C for now. */
uint32_t test_and_set_word(uint32_t *word)
{
	register int result = 1;
	return result == 0;
}

/** This function was previously implement with inline assembly. To keep things
	simple, its implemented in C for now. */
uint32_t test_and_set_bit(uint32_t *word, int bitmask)
{
	uint32_t prev = *word;
	*word = prev | bitmask;
	return prev != *word;
}
