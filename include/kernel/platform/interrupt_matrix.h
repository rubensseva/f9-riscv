#ifndef INTERRUPT_MATRIX_H_
#define INTERRUPT_MATRIX_H_

#include <stdint.h>

/**
   These functions operate on the ESP32-C3 interrupt matrix. Additional configuration on the source
   registers for the different interrupts is needed for the interrupts to work properly.

   The configuration is a bit complex by the fact that the ESP32-C3 has 62 peripheral interrupts, which can
   be mapped to 32 CPU interrupts.

   Note that in order to clear an interrupt using the interrupt matrix, the interrupt MUST be configured
   to be a edge type interrupt. When the type is an edge type interrupt, it is usually required to clear
   the interrupt both in the interrupt matrix and from the source registers.
 */


/**
   Set the priority of a CPU interrupt. Priority can be from 0-15.
   Note that if the priority is below the priority threshold (configured in a different
   register), then the interrupt will be masked.
*/
void intr_set_priority(int cpu_intr_n, int pri);

/**
   Set the type for a CPU interrupt.
   The type argument should be 0 for level type, 1 for edge type interrupt.
   Note that a level type interrupt cannot be cleared by the interrupt matrix, it must
   be cleared from source.
*/
void intr_set_type(int cpu_intr_n, int type);

/**
   Map source interrupt to cpu interrupt.
   The source_intr pointer is the address of the map register for the source interrupt, relative
   to the interrupt matrix base register.
   The map registers are listed in ESP32-C3 TRM section 8.4
*/
void intr_map(uint32_t *source_intr, int cpu_intr_n);

/**
   Enable a cpu interrupt.
   Note that additional configuration is needed for the interrupt to work, including mapping the source
   interrupt to a cpu interrupt, and enabling the interrupt from source.
*/
void intr_en(int cpu_intr_n);

/**
   Clear a CPU interrupt.
   Note that this will only work if the interrupt is configured to be a edge type interrupt
*/
void intr_clear(int cpu_intr_n);

/**
   Set up the interrupt matrix for a CPU interrupt.
*/
void intr_setup(unsigned int cpu_intr_n, unsigned int priority);

#endif // INTERRUPT_MATRIX_H_
