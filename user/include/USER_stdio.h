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

/* Literal strings are placed in .rodata segment, which is kernel
   space, so we need this hackery to be able to print literal strings */
#define user_printf(str, ...) { \
    static __USER_DATA char s[] = str; \
    printf(s, __VA_ARGS__); \
}
#define user_puts(str) { \
    static __USER_DATA char s[] = str; \
    printf(s); \
}

void puts(char *str);
void printf(char *fmt, ...);
void vprintf(char *fmt, va_list va);


#endif // USER_STDIO_H_
