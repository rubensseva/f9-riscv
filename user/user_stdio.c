#include <user_stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <uart_ESP32_C3.h>

#include <user_thread.h>
#include <link.h>


/* TODO: In the original F9 code, this function is part of a debug device abstraction.
 * That abstraction should be added here as well */
__USER_TEXT void putchar(uint8_t c)
{
    UART_write(c, 0);
}

__USER_TEXT void puts(char *str)
{
    while (*str) {
        if (*str == '\n')
            putchar('\r');
        putchar(*(str++));
    }
}

__USER_TEXT static void puts_x(char *str, int width, const char pad)
{
    while (*str) {
        if (*str == '\n')
            putchar('\r');
        putchar(*(str++));
        --width;
    }

    while (width > 0) {
        putchar(pad);
        --width;
    }
}

#define hexchars(x)             \
    (((x) < 10) ?           \
        ('0' + (x)) :       \
        ('a' + ((x) - 10)))

__USER_TEXT static int put_hex(const uint32_t val, int width, const char pad)
{
    int i, n = 0;
    int nwidth = 0;

    /* Find width of hexnumber */
    while ((val >> (4 * nwidth)) && ((unsigned) nwidth <  2 * sizeof(val)))
        nwidth++;
    if (nwidth == 0)
        nwidth = 1;

    /* May need to increase number of printed characters */
    if (width == 0 && width < nwidth)
        width = nwidth;

    /* Print number with padding */
    for (i = width - nwidth; i > 0; i--, n++)
        putchar(pad);
    for (i = 4 * (nwidth - 1); i >= 0; i -= 4, n++)
        putchar(hexchars((val >> i) & 0xF));

    return n;
}

__USER_TEXT static void put_dec(const uint32_t val, const int width, const char pad)
{
    uint32_t divisor;
    int digits;

    /* estimate number of spaces and digits */
    for (divisor = 1, digits = 1; val / divisor >= 10; divisor *= 10, digits++)
        /* */ ;

    /* print spaces */
    for (; digits < width; digits++)
        putchar(pad);

    /* print digits */
    do {
        putchar(((val / divisor) % 10) + '0');
    } while (divisor /= 10);
}


__USER_TEXT void printf(char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    vprintf(fmt, va);
    va_end(va);
}

__USER_TEXT void vprintf(char *fmt, va_list va)
{
    int mode = 0;   /* 0: usual char; 1: specifiers */
    int width = 0;
    char pad = ' ';
    int size = 16;

    while (*fmt) {
        if (*fmt == '%') {
            mode = 1;
            pad = ' ';
            width = 0;
            size = 32;

            fmt++;
            continue;
        }

        if (!mode) {
            if (*fmt == '\n')
                putchar('\r');
            putchar(*fmt);
        } else {
            switch (*fmt) {
            case 'c':
                putchar(va_arg(va, uint32_t));
                mode = 0;
                break;
            case 's':
                puts_x(va_arg(va, char *), width, pad);
                mode = 0;
                break;
            case 'l':
            case 'L':
                size = 64;
                break;
            case 'd':
            case 'D':
                put_dec((size == 32) ?
                             va_arg(va, uint32_t) :
                             va_arg(va, uint64_t),
                             width, pad);
                mode = 0;
                break;
            case 'p':
            case 't':
                size = 32;
                width = 8;
                pad = '0';
            case 'x':
            case 'X':
                put_hex((size == 32) ?
                             va_arg(va, uint32_t) :
                             va_arg(va, uint64_t),
                             width, pad);
                mode = 0;
                break;
            case '%':
                putchar('%');
                mode = 0;
                break;
            case '0':
                if (!width)
                    pad = '0';
                break;
            case ' ':
                pad = ' ';
            }

            if (*fmt >= '0' && *fmt <= '9') {
                width = width * 10 + (*fmt - '0');
            }
        }

        fmt++;
    }
}
