#include <stdint.h>
#include <ESP32_C3.h>
#include <utility.h>
#include <config.h>

#include <platform/interrupt_matrix.h>


void intr_set_priority(int cpu_intr_n, int pri) {
    volatile uint32_t *interrupt_core0_cpu_int_pri = REG(INTERRUPT_MATRIX_BASE + INTERRUPT_CORE0_CPU_INT_PRI_n_REG + 0x4 * (cpu_intr_n - 1));
    *interrupt_core0_cpu_int_pri = pri;
}

void intr_set_type(int cpu_intr_n, int type) {
    /* Set the type to edge triggered, so we can clear it with the interrupt matrix */
    volatile uint32_t *interrupt_core0_cpu_int_type = REG(INTERRUPT_MATRIX_BASE + INTERRUPT_CORE0_CPU_INT_TYPE_REG);
    if (type) {
        *interrupt_core0_cpu_int_type |= (1 << cpu_intr_n); // set edge triggered
    } else {
        *interrupt_core0_cpu_int_type &= ~(1 << cpu_intr_n); // set level triggered
    }
}

void intr_map(uint32_t *source_intr, int cpu_intr_n) {
    volatile uint32_t *interrupt_core0_intr_map_reg = REG(INTERRUPT_MATRIX_BASE + (uint32_t) source_intr);
    *interrupt_core0_intr_map_reg = cpu_intr_n;
}

void intr_en(int cpu_intr_n) {
    volatile uint32_t *interrupt_core0_cpi_in_enable = REG(INTERRUPT_MATRIX_BASE + INTERRUPT_CORE0_CPU_INT_ENABLE_REG);
    *interrupt_core0_cpi_in_enable |= (1 << cpu_intr_n);
}

void intr_clear(int cpu_intr_n) {
    /* Clear uart interrupt in interrupt matrix (not sure if this is necessary, but lets do it to be sure) */
    volatile uint32_t *interrupt_core0_cpu_int_clear = REG(INTERRUPT_MATRIX_BASE + INTERRUPT_CORE0_CPU_INT_CLEAR_REG);
    *interrupt_core0_cpu_int_clear |= (1 << cpu_intr_n);
    *interrupt_core0_cpu_int_clear &= ~(1 << cpu_intr_n);
}

void UART_receive_intr_matr_init()
{
    /* Map UART interrupt */
    /* TODO: Should this be reg UART, or UART1? */
    /* volatile uint32_t *interrupt_core0_systimer_target0_int_map_reg = REG(INTERRUPT_MATRIX_BASE + INTERRUPT_CORE0_UART_TARGET0_INT_MAP_REG); */
    /* *interrupt_core0_systimer_target0_int_map_reg = CONFIG_UART_CPU_INTR; */
    intr_map((uint32_t *)INTERRUPT_CORE0_UART_TARGET0_INT_MAP_REG, CONFIG_UART_CPU_INTR);

    /* Set the priority of the interrupt */
    /* volatile uint32_t *interrupt_core0_cpu_int_pri = REG(INTERRUPT_MATRIX_BASE + INTERRUPT_CORE0_CPU_INT_PRI_n_REG + 0x4 * (CONFIG_UART_CPU_INTR - 1)); */
    /* *interrupt_core0_cpu_int_pri = 11; // Set hightes priority for now TODO: Set a more sensible priority */
    intr_set_priority(CONFIG_UART_CPU_INTR, 11);

    /* Set the type to edge triggered, so we can clear it with the interrupt matrix */
    /* volatile uint32_t *interrupt_core0_cpu_int_type = REG(INTERRUPT_MATRIX_BASE + INTERRUPT_CORE0_CPU_INT_TYPE_REG); */
    /* *interrupt_core0_cpu_int_type |= (1 << CONFIG_UART_CPU_INTR); */
    intr_set_type(CONFIG_UART_CPU_INTR, 1);

    /* After mapping system timer to CPU interrupt, we enable the CPU interrupt number */
    /* volatile uint32_t *interrupt_core0_cpi_in_enable = REG(INTERRUPT_MATRIX_BASE + INTERRUPT_CORE0_CPU_INT_ENABLE_REG); */
    /* *interrupt_core0_cpi_in_enable |= (1 << CONFIG_UART_CPU_INTR); */
    intr_en(CONFIG_UART_CPU_INTR);

    /* Clear uart interrupt in interrupt matrix (not sure if this is necessary, but lets do it to be sure) */
    volatile uint32_t *interrupt_core0_cpu_int_clear = REG(INTERRUPT_MATRIX_BASE + INTERRUPT_CORE0_CPU_INT_CLEAR_REG);
    *interrupt_core0_cpu_int_clear |= (1 << CONFIG_UART_CPU_INTR);
    *interrupt_core0_cpu_int_clear &= ~(1 << CONFIG_UART_CPU_INTR);
    intr_clear(CONFIG_UART_CPU_INTR);
}
