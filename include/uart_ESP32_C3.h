#ifndef UARTESP32_C3_H
#define UARTESP32_C3_H

void UART_init();
void UART_conf();
void UART_write(char c);

void UART_init_2(int tx, int rx);
int UART_read_2(char *c);
void UART_write_2(char c);

void UART_init_3(int baud);
void UART_write_3(char c);

#endif /* UARTESP32_C3_H */
