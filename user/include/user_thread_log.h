#ifndef USER_LOG_H_
#define USER_LOG_H_

int __user_log_printf(const char *format, ...);
void __user_log_putc(int c);

/* Literal strings are placed in .rodata section, which is kernel
   space, so we need this hackery to be able to print literal strings */
#define user_log_printf(str, ...) { \
    static __USER_DATA char s[] = str; \
    __user_log_printf(s, __VA_ARGS__); \
}

#define user_log_puts(str) { \
    static __USER_DATA char s[] = str; \
    __user_log_printf(s); \
}

#endif // USER_LOG_H_
