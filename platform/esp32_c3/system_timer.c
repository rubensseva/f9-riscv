#include <stdint.h>
#include <platform/ESP32_C3.h>
#include <config.h>
#include <utility.h>

void system_timer_init() {
    /* The counters and comparators are driven by XTAL_CLK. After scaled by a fractional
       divider, the average clock frequency is 16MHz. */


    /* Following the steps in 10.5.3 in TRM */
    /* 1. Set SYSTIMER_TARGETx_TIMER_UNIT_SEL to select the counter (UNIT0 or UNIT1) used for COMPx. */
    /* NOTE: We will set this to 0, which I believe will select UNIT0 */
    volatile uint32_t *systimer_target0_conf = REG(SYSTEM_TIMER_BASE + SYSTIMER_TARGET0_CONF_REG);
    *systimer_target0_conf &= ~(1 << SYSTIMER_TARGET0_CONF_REG__SYSTIMER_TARGET0_TIMER_UNIT_SEL);
    /* 2. Set an alarm period (δt), and fill it to SYSTIMER_TARGETx_PERIOD. */
    /* The counters run at 16MHz, so setting this 16000000 wile give onw interrupt each second */
    int alarm = CONFIG_SYSTEM_TIMER_ALARM_THRESH;
    *systimer_target0_conf &= ~0x3ffffff; // zero out whatever is in period field (bits 0 - 25)
    *systimer_target0_conf |= alarm;
    /* 3. Set SYSTIMER_TIMER_COMPx_LOAD to synchronize the alarm period (δt) to COMPx, i.e. load the alarm period (δt) to COMPx. */
    volatile uint32_t *systimer_comp0_load = REG(SYSTEM_TIMER_BASE + SYSTIMER_COMP0_LOAD_REG);
    *systimer_comp0_load = 1;
    /* 4. Set SYSTIMER_TARGETx_PERIOD_MODE to configure COMPx into period mode. */
    *systimer_target0_conf |= (1 << SYSTIMER_TARGET0_CONF_REG__SYSTIMER_TARGET0_PERIOD_MODE);
    /* 5. Set SYSTIMER_TARGETx_WORK_EN to enable the selected COMPx. COMPx starts comparing the count value with the sum of start value + n*δt (n = 1, 2, 3...). */
    volatile uint32_t *systimer_conf_reg = REG(SYSTEM_TIMER_BASE + SYSTIMER_CONF_REG);
    *systimer_conf_reg |= (1 << SYSTIMER_CONF_REG__SYSTIMER_TARGET0_WORK_EN);
    /* 6. Set SYSTIMER_TARGETx_INT_ENA to enable timer interrupt. A SYSTIMER_TARGETx_INT interrupt is triggered when Unitn counts to start value + n*δt (n = 1, 2, 3...) set in step 2. */
    volatile uint32_t *systimer_int_ena = REG(SYSTEM_TIMER_BASE + SYSTIMER_INT_ENA_REG);
    *systimer_int_ena |= 1; // enable interrups for target 0

    /* Map peripheral interrupt system timer to a CPU interrupt number */
    volatile uint32_t *interrupt_core0_systimer_target0_int_map_reg = REG(INTERRUPT_MATRIX_BASE + INTERRUPT_CORE0_SYSTIMER_TARGET0_INT_MAP_REG);
    *interrupt_core0_systimer_target0_int_map_reg = CONFIG_SYSTEM_TIMER_CPU_INTR;

    /* Set interrupt type to level-type interrupt, as stated in TRM 10.4.4. */
    volatile uint32_t *interrupt_core0_cpu_int_type = REG(INTERRUPT_MATRIX_BASE + INTERRUPT_CORE0_CPU_INT_TYPE_REG);
    *interrupt_core0_cpu_int_type &= ~(1 << CONFIG_SYSTEM_TIMER_CPU_INTR); // set to level type by writing zero

    /* Set the priority of the interrupt */
    volatile uint32_t *interrupt_core0_cpu_int_pri = REG(INTERRUPT_MATRIX_BASE + INTERRUPT_CORE0_CPU_INT_PRI_n_REG + 0x4 * (CONFIG_SYSTEM_TIMER_CPU_INTR - 1));
    *interrupt_core0_cpu_int_pri = 10; // Set hightes priority for now TODO: Set a more sensible priority

    /* Set the threshold of interrupt priorities to 1, so that all interrupts are taken except those with priority = 0 */
    /* TODO: This should be in a more general interrupt_init() type of function */
    volatile uint32_t *interrupt_core0_cpu_int_thresh = REG(INTERRUPT_MATRIX_BASE + INTERRUPT_CORE0_CPU_INT_THRESH_REG);
    *interrupt_core0_cpu_int_thresh = 1;

    /* After mapping system timer to CPU interrupt, we enable the CPU interrupt number */
    volatile uint32_t *interrupt_core0_cpi_in_enable = REG(INTERRUPT_MATRIX_BASE + INTERRUPT_CORE0_CPU_INT_ENABLE_REG);
    *interrupt_core0_cpi_in_enable |= (1 << CONFIG_SYSTEM_TIMER_CPU_INTR);


    /* And finally start the counter */
    *systimer_conf_reg |= (1 << SYSTIMER_CONF_REG__SYSTIMER_TIMER_UNIT0_WORK_EN);
}
