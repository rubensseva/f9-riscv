#include <stdint.h>
#include <link.h>
#include <uart_ESP32_C3.h>
#include <ESP32_C3.h>
#include <config.h>
#include <utility.h>


__USER_TEXT void UART_init(int baud, int controller_num)
{
    uint32_t controller = controller_num ? UART_CONTROLLER_1_BASE : UART_CONTROLLER_0_BASE;
    volatile uint32_t *uart_clk_div = REG(controller + UART_CLKDIV_REG);
    uint32_t div = (uint32_t) (4000U * 1000000 / (uint32_t) baud);
    *uart_clk_div = div / 100;                        // Integral part
    *uart_clk_div |= (16 * (div % 100) / 100) << 20;  // Fractional part

    volatile uint32_t *uart_conf0_reg = REG(controller + UART_CONF0_REG);
    *uart_conf0_reg &= ~BIT(28);
    *uart_conf0_reg |= BIT(26);
    volatile uint32_t *uart_clk_conf = REG(controller + UART_CLK_CONF_REG);
    *uart_clk_conf = BIT(25) | BIT(24) | BIT(22) | BIT(21) | BIT(20);  // Use APB NOTE: Is this APB? Seems to be XTAL_CLK...
}

__USER_TEXT void UART_receive_init(int controller_num)
{
    uint32_t controller = controller_num ? UART_CONTROLLER_1_BASE : UART_CONTROLLER_0_BASE;
    /* Set receive threshold to 1, so that we generate an interrupt when receiving one character */
    volatile uint32_t *uart_conf1 = REG(controller + UART_CONF1_REG);
    *uart_conf1 &= ~(0xFF);
    *uart_conf1 |= (1 << UART_CONF1_REG__UART_RXFIFO_FULL_THRHD);

    /* Enable UART interrupt when RXFIFO is full */
    volatile uint32_t *uart_int_ena = REG(controller + UART_INT_ENA_REG);
    *uart_int_ena |= (1 << UART_INTR__UART_RXFIFO_FULL);

    /* Map UART interrupt */
    /* TODO: Should this be reg UART, or UART1? */
    volatile uint32_t *interrupt_core0_systimer_target0_int_map_reg = REG(INTERRUPT_MATRIX_BASE + INTERRUPT_CORE0_UART_TARGET0_INT_MAP_REG);
    *interrupt_core0_systimer_target0_int_map_reg = CONFIG_UART_CPU_INTR;

    /* Set the priority of the interrupt */
    volatile uint32_t *interrupt_core0_cpu_int_pri = REG(INTERRUPT_MATRIX_BASE + INTERRUPT_CORE0_CPU_INT_PRI_n_REG + 0x4 * (CONFIG_UART_CPU_INTR - 1));
    *interrupt_core0_cpu_int_pri = 11; // Set hightes priority for now TODO: Set a more sensible priority

    /* Set the type to edge triggered, so we can clear it with the interrupt matrix */
    volatile uint32_t *interrupt_core0_cpu_int_type = REG(INTERRUPT_MATRIX_BASE + INTERRUPT_CORE0_CPU_INT_TYPE_REG);
    *interrupt_core0_cpu_int_type |= (1 << CONFIG_UART_CPU_INTR);

    /* After mapping system timer to CPU interrupt, we enable the CPU interrupt number */
    volatile uint32_t *interrupt_core0_cpi_in_enable = REG(INTERRUPT_MATRIX_BASE + INTERRUPT_CORE0_CPU_INT_ENABLE_REG);
    *interrupt_core0_cpi_in_enable |= (1 << CONFIG_UART_CPU_INTR);

    /* Clear uart interrupt in interrupt matrix (not sure if this is necessary, but lets do it to be sure) */
    volatile uint32_t *interrupt_core0_cpu_int_clear = REG(INTERRUPT_MATRIX_BASE + INTERRUPT_CORE0_CPU_INT_CLEAR_REG);
    *interrupt_core0_cpu_int_clear |= (1 << CONFIG_UART_CPU_INTR);
    *interrupt_core0_cpu_int_clear &= ~(1 << CONFIG_UART_CPU_INTR);

    /* Clear uart interrupt from source */
    volatile uint32_t *uart_int_clr = REG(UART_CONTROLLER_0_BASE + UART_INT_CLR_REG);
    *uart_int_clr |= (1 << UART_INTR__UART_RXFIFO_FULL);
};

__USER_TEXT uint32_t UART_txfifo_count(int controller_num) {
    uint32_t controller = controller_num ? UART_CONTROLLER_1_BASE : UART_CONTROLLER_0_BASE;
    volatile uint32_t *uart_status_reg = REG(controller + UART_STATUS_REG);
    uint32_t val = ((*uart_status_reg) >> UART_STATUS_REG__UART_TXFIFO_CNT_BIT);
    return val & 0x3ff;
}

__USER_TEXT uint32_t UART_rxfifo_count(int controller_num) {
    uint32_t controller = controller_num ? UART_CONTROLLER_1_BASE : UART_CONTROLLER_0_BASE;
    volatile uint32_t *uart_status_reg = REG(controller + UART_STATUS_REG);
    uint32_t val = ((*uart_status_reg) >> UART_STATUS_REG__UART_RXFIFO_CNT_BIT);
    return val & 0x3ff;
}

__USER_TEXT void UART_write(char c, int controller_num)
{
    uint32_t controller = controller_num ? UART_CONTROLLER_1_BASE : UART_CONTROLLER_0_BASE;
    volatile uint32_t *uart_rxfifo_rd_byte = REG(controller + UART_FIFO_REG);
    while (UART_txfifo_count(controller_num) != 0) {}
    *uart_rxfifo_rd_byte = c;
}

__USER_TEXT char UART_read(int controller_num)
{
    uint32_t controller = controller_num ? UART_CONTROLLER_1_BASE : UART_CONTROLLER_0_BASE;
    volatile uint32_t *uart_rxfifo_rd_byte = REG(controller + UART_FIFO_REG);
    return (char) *uart_rxfifo_rd_byte;
}
