#include <user_thread_log.h>
#include <F9.h>

__USER_TEXT void ping() {
    user_log_puts("ping start\n");
    while (1) {
        /* TODO: Need to use proper timeout object here */
        L4_Sleep(100000);
    }
}


__USER_TEXT void pong() {
    user_log_puts("pong start\n");
    while (1) {
        /* TODO: Need to use proper timeout object here */
        L4_Sleep(100000);
    }
}
