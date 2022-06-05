#include <message.h>
#include <ping_pong.h>
#include <user_thread_log.h>
#include <user_types.h>
#include <F9.h>
#include <timer_ESP32_C3.h>

__USER_DATA L4_ThreadId_t ping_id;
__USER_DATA L4_ThreadId_t pong_id;
__USER_DATA L4_MsgTag_t ping_pong_tag;
__USER_DATA L4_MsgTag_t ping_pong_res_tag;

__USER_TEXT void print_msg(L4_Msg_t *msg) {
    user_log_printf("tag - u: %d n: %d\n",
                    msg->tag.X.u, msg->tag.X.t);
    int i;
    for (i = 1; i < msg->tag.X.u; i++)
        user_log_printf("%d: untyped, %d\n", i, msg->msg[i]);
    for (; i < msg->tag.X.u + msg->tag.X.t; i++)
        user_log_printf("%d: typed, %d\n", i, msg->msg[i]);
}

/* This one fails because memcpy is in kernel space */
__USER_TEXT void print_msg_cpy(L4_Msg_t msg) {
    user_log_printf("tag - u: %d n: %d\n",
                    msg.tag.X.u, msg.tag.X.t);
    int i;
    for (i = 1; i < msg.tag.X.u; i++)
        user_log_printf("%d: untyped, %d\n", i, msg.msg[i]);
    for (; i < msg.tag.X.u + msg.tag.X.t; i++)
        user_log_printf("%d: typed, %d\n", i, msg.msg[i]);
}

__USER_TEXT void ping() {
    user_log_puts("ping start\n");
    timer_init();

    TIMER_START();
    for (int i = 0; i < 1000; i++) {
        /* Initial message */
        L4_Msg_t msg;
        L4_MsgClear(&msg);

        L4_MsgAppendWord(&msg, 1);
        L4_MsgAppendWord(&msg, 2);
        L4_MsgAppendWord(&msg, 3);
        ping_pong_tag = msg.tag;

        /* print_msg(&msg); */
        /* user_log_printf("ping sending %d, %d, %d, %d\n", */
        /*                 msg.tag, msg.msg[1], msg.msg[2], msg.msg[3]); */

        L4_MsgLoad(&msg);
        L4_Ipc(pong_id, ping_id, 0, (L4_ThreadId_t *)0);



        /* Response */
        L4_MsgClear(&msg);
        L4_MsgStore(ping_pong_res_tag, &msg);
        /* user_log_printf("ping got response %d, %d, %d, %d\n", */
        /*                 msg.tag, msg.msg[1], msg.msg[2], msg.msg[3]); */
    }
    TIMER_LATCH();
    user_log_puts("ping done\n");
    int counter_val = timer_get();
    int us = timer_counter_to_microseconds(counter_val);
    user_log_printf("time: %d\n", us);

    while (1) {
        /* TODO: Need to use proper timeout object here */
        L4_Sleep(100000);
    }
}


__USER_TEXT void pong() {
    user_log_puts("pong start\n");

    for (int i = 0; i < 1000; i++) {
        L4_Msg_t msg;

        /* Initial message */
        L4_Ipc(L4_nilthread, ping_id, 0, (L4_ThreadId_t *)0);
        L4_MsgStore(ping_pong_tag, &msg);
        /* user_log_printf("pong received %d, %d, %d, %d\n", */
        /*                 msg.tag, msg.msg[1], msg.msg[2], msg.msg[3]); */


        /* Response */
        L4_MsgClear(&msg);
        L4_MsgAppendWord(&msg, 6);
        L4_MsgAppendWord(&msg, 7);
        L4_MsgAppendWord(&msg, 8);
        ping_pong_res_tag = msg.tag;

        /* user_log_printf("pong responding with %d, %d, %d, %d\n", */
        /*                 msg.tag, msg.msg[1], msg.msg[2], msg.msg[3]); */

        L4_MsgLoad(&msg);
        L4_Ipc(ping_id, L4_nilthread, 0, (L4_ThreadId_t *)0);
    }
    user_log_puts("pong done\n");

    while (1) {
        /* TODO: Need to use proper timeout object here */
        L4_Sleep(100000);
    }
}
