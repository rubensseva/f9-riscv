#include <ping_pong.h>
#include <user_thread_log.h>
#include <user_types.h>
#include <F9.h>


L4_ThreadId_t ping_id;
L4_ThreadId_t pong_id;

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
