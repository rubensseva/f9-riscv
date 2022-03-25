/* Copyright (c) 2013 The F9 Microkernel Project. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef PLATFORM_MPU_H_
#define PLATFORM_MPU_H_

#include <memory.h>

void mpu_setup_region(int n, fpage_t *fp);
void mpu_enable(mpu_state_t i);
void __memmanage_handler(void);
int mpu_select_lru(as_t *as, uint32_t addr);

void w_pmpaddri(int pmp_entry, uint32_t data);
void w_pmpcfgi_region(int i, uint8_t data, uint32_t old_data);

void map_intr_vector(uint32_t vector_table_ptr);

void dump_mpu();

#endif  /* PLATFORM_MPU_H_ */
