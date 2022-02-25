#ifndef UART_H_
#define UART_H_

void uartinit(void);
void uartputc(int c);
void uartputc_sync(int c);
void uartstart();
int uartgetc(void);
void uartintr(void);

#endif // UART_H_
