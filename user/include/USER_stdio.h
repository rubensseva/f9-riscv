#ifndef USER_STDIO_H_
#define USER_STDIO_H_

#include <types.h>
#include <USER_stdarg.h>

#define EOF     (-1)

#ifdef CONFIG_STDIO_NODEV
#define putchar(chr)    do { } while (0)
#define getchar()       (EOF)
#else
void putchar(uint8_t chr);
uint8_t getchar(void);
#endif

void puts(char *str);
void printf(char *fmt, ...);
void vprintf(char *fmt, va_list va);


#endif // USER_STDIO_H_
