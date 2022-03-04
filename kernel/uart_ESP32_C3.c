#include <stdint.h>
#include <link.h>
#include <uart_ESP32_C3.h>
#include <ESP32_C3.h>
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