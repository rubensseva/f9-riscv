#ifndef UART_ESP32_C3_H
#define UART_ESP32_C3_H

#include <stdint.h>

void UART_init(int baud, int controller_num);
void UART_receive_init(int controller_num);
uint32_t UART_txfifo_count(int controller_num);
uint32_t UART_rxfifo_count(int controller_num);
void UART_write(char c, int controller_num);
char UART_read(int controller_num);
void UART_clear(int controller_num);

#endif /* UART_ESP32_C3_H */
