#include <stdint.h>
#include <link.h>
#include <uart_ESP32_C3.h>
#include <ESP32_C3.h>
#include <config.h>
#include <utility.h>


__USER_TEXT void UART_init(int baud)
{
    volatile uint32_t *uart_clk_div = REG(UART_CONTROLLER_0_BASE + UART_CLKDIV_REG);
    uint32_t div = (uint32_t) (4000U * 1000000 / (uint32_t) baud);
    *uart_clk_div = div / 100;                        // Integral part
    *uart_clk_div |= (16 * (div % 100) / 100) << 20;  // Fractional part

    volatile uint32_t *uart_conf0_reg = REG(UART_CONTROLLER_0_BASE + UART_CONF0_REG);
    *uart_conf0_reg &= ~BIT(28);
    *uart_conf0_reg |= BIT(26);
    volatile uint32_t *uart_clk_conf = REG(UART_CONTROLLER_0_BASE + UART_CLK_CONF_REG);
    *uart_clk_conf = BIT(25) | BIT(24) | BIT(22) | BIT(21) | BIT(20);  // Use APB NOTE: Is this APB? Seems to be XTAL_CLK...
}

__USER_TEXT void UART_receive_init() {
    /* Set receive threshold to 1, so that we generate an interrupt when receiving one character */
    volatile uint32_t *uart_conf1 = REG(UART_CONTROLLER_0_BASE + UART_CONF1_REG);
    *uart_conf1 &= ~(0xFF);
    *uart_conf1 |= (1 << UART_CONF1_REG__UART_RXFIFO_FULL_THRHD);

    /* Map UART interrupt */
    /* TODO: Should this be reg UART, or UART1? */
    volatile uint32_t *interrupt_core0_systimer_target0_int_map_reg = REG(INTERRUPT_MATRIX_BASE + INTERRUPT_CORE0_UART_TARGET0_INT_MAP_REG);
    *interrupt_core0_systimer_target0_int_map_reg = CONFIG_UART_CPU_INTR;

    /* Set the priority of the interrupt */
    volatile uint32_t *interrupt_core0_cpu_int_pri = REG(INTERRUPT_MATRIX_BASE + INTERRUPT_CORE0_CPU_INT_PRI_n_REG + 0x4 * (CONFIG_UART_CPU_INTR - 1));
    *interrupt_core0_cpu_int_pri = 11; // Set hightes priority for now TODO: Set a more sensible priority

    /* After mapping system timer to CPU interrupt, we enable the CPU interrupt number */
    volatile uint32_t *interrupt_core0_cpi_in_enable = REG(INTERRUPT_MATRIX_BASE + INTERRUPT_CORE0_CPU_INT_ENABLE_REG);
    *interrupt_core0_cpi_in_enable |= (1 << CONFIG_SYSTEM_TIMER_CPU_INTR);
}

__USER_TEXT void UART_write(char c)
{
    volatile uint32_t *uart_rxfifo_rd_byte = REG(UART_CONTROLLER_0_BASE + UART_FIFO_REG);
    volatile uint32_t *uart_status_reg = REG(UART_CONTROLLER_0_BASE + UART_STATUS_REG);

    uint32_t val;
    do {
        val = ((*uart_status_reg) >> UART_STATUS_REG__UART_TXFIFO_CNT_BIT);
    } while((val & 0x3ff) != 0);

    *uart_rxfifo_rd_byte = c;
}
