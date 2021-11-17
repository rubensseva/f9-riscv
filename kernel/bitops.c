/* Copyright (c) 2013 The F9 Microkernel Project. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

// #include <platform/armv7m.h>
// #include <platform/bitops.h>
#include <stdint.h>

#ifdef CONFIG_SMP

/* Atomic ops */
void atomic_set(atomic_t *atom, atomic_t newval)
{
	/* __asm__ __volatile__("mov r1, %0" : : "r"(newval)); */
	/* __asm__ __volatile__("atomic_try: ldrex r0, [%0]\n" */
	/*                      "strex r0, r1, [%0]\n" */
	/*                      "cmp r0, #0" */
	/*                      : */
	/*                      : "r"(atom)); */
	/* __asm__ __volatile__("bne atomic_try"); */
}

uint32_t atomic_get(atomic_t *atom)
{
	atomic_t result;

	/* __asm__ __volatile__("ldrex r0, [%0]" */
	/*                      : */
	/*                      : "r"(atom)); */
	/* __asm__ __volatile__("clrex"); */
	/* __asm__ __volatile__("mov %0, r0" : "=r"(result)); */

	return result;
}

#else	/* !CONFIG_SMP */

/* void atomic_set(atomic_t *atom, atomic_t newval) */
/* { */
/* 	*atom = newval; */
/* } */

/* uint32_t atomic_get(atomic_t *atom) */
/* { */
/* 	return *atom; */
/* } */

#endif	/* CONFIG_SMP */

uint32_t test_and_set_word(uint32_t *word)
{
	register int result = 1;

	/* __asm__ __volatile__( */
	/*     "mov r1, #1\n" */
	/*     "mov r2, %[word]\n" */
	/*     "ldrex r0, [r2]\n"	/\* Load value [r2] *\/ */
	/*     "cmp r0, #0\n"	/\* Checking is word set to 1 *\/ */

	/*     "itt eq\n" */
	/*     "strexeq r0, r1, [r2]\n" */
	/*     "moveq %[result], r0\n" */
	/*     : [result] "=r"(result) */
	/*     : [word] "r"(word) */
	/*     : "r0", "r1", "r2"); */

	return result == 0;
}

// I think bitmask is just one bit set.
// So basically we check if word has the bit(s) in bitmask set.
// the tst instruction does a bitwise AND, updates flags, discards result
// So what will happen is that tst could set zero flag, depending
// on usage.
//
// What this function does:
// Check if the bit(s) in bitmask is already set
// if they are set, return.
// If thery are not set, set them, then return.
// In the end, strex is used to store back the value, and the status of the store
// is put in r0. r0 will now be 0 if the store was successful. So return r0 == result, which
// means that the function will return true (1) if the store was successful
//
// For the port, lets keep things simple and just set
// the necessary bits
uint32_t test_and_set_bit(uint32_t *word, int bitmask)
{
	// register int result = 0;

	uint32_t prev = *word;
	*word = prev | bitmask;
	return prev != *word;

	/* __asm__ __volatile__( */
	/* 	"mv a2, %[word]\n"           /\* load word in a2 *\/ */
	/* 	"lw a3, 0(a2)\n"             /\* load value at address a2 in a3 *\/ */
	/* 	"or a4, a3, %[bitmask]\n"    /\* ori value in a3 with bitmask *\/ */
	/* 	"sub a5, a4, a3\n" */
	/* 	"sw a4, 0(a2)\n"             /\* store the orred value back in address in a2 *\/ */
	/* 	"mv %[result], 1\n"          /\* store the result of the andi in the "result" variable. if it is zero, an edit was made. *\/ */
	/* 	"skip:\n" */
	/*     : [result] "=r"(result) */
	/*     : [word] "r"(word), [bitmask] "r"(bitmask) */
	/*     : "a2", "a3", "a4", "a5"); */

	/* __asm__ __volatile__( */
	/*     "mov r2, %[word]\n"      /\* mov word into r2 *\/ */
	/*     "ldrex r0, [r2]\n"		/\* Load value [r2] *\/ */
	/*     "tst r0, %[bitmask]\n"	/\* Compare value with bitmask *\/ */

	/* 	// These should be taken if zero flag is set */
	/*     "ittt eq\n" */
	/*     "orreq r1, r0, %[bitmask]\n"	/\* Set bit: r1 = r0 | bitmask *\/ */
	/*     "strexeq r0, r1, [r2]\n"		/\* Write value back to [r2] *\/ */
	/*     "moveq %[result], r0\n" */
	/*     : [result] "=r"(result) */
	/*     : [word] "r"(word), [bitmask] "r"(bitmask) */
	/*     : "r0", "r1", "r2"); */

	// return result;
}
