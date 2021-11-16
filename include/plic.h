/* Copyright (c) 2014 The F9 Microkernel Project. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef __PLATFORM_STM32F1_NVIC_H__
#define __PLATFORM_STM32F1_NVIC_H__


#define IRQ_ENABLE	1
#define IRQ_DISABLE	0

void plicinit(void);
void plicinithart(void);
int plic_claim(void);
void plic_complete(int irq);

typedef enum IRQn {
	USART1_IRQn		= 0,	/*!< USART1 global Interrupt */
	IRQn_NUM,
} IRQn_Type;

#define MAX_IRQn FPU_IRQn

/* Define device IRQ handler name */
#define USART1_HANDLER              nvic_handler0

#define IRQ_VEC_N_HANDLER_DECLARE(n) \
	void nvic_handler##n (void);

#define IRQ_VEC_N_OP	IRQ_VEC_N_HANDLER_DECLARE
#include <plic_private.h>
#undef IRQ_VEC_N_OP

#endif
