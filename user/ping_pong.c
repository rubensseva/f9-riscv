#include <message.h>
#include <ping_pong.h>
#include <user_thread_log.h>
#include <user_types.h>
#include <F9.h>
#include <ESP32_C3_timer.h>

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
    L4_Msg_t msg_out, msg_in;
    L4_Word_t msgs[16] = {
        1,
        2,
        3,
        4,
        5,
        6,
        7,
        8,
        9,
        10,
        11,
        12,
        13,
        14,
        15,
    };
    L4_MsgClear(&msg_out);
    L4_MsgPut(&msg_out, 0, 15, msgs, 0, NULL);
    ping_pong_tag = msg_out.tag;


    timer_init();
    TIMER_START();
    for (int i = 0; i < 100000; i++) {
        /* Initial message */
        /* msg_in.msg[1] = 0; */
        /* msg_in.msg[2] = 0; */
        /* msg_in.msg[3] = 0; */
        /* msg_in.msg[4] = 0; */
        /* msg_in.msg[5] = 0; */
        /* msg_in.msg[6] = 0; */
        /* msg_in.msg[7] = 0; */
        /* msg_in.msg[8] = 0; */
        /* msg_in.msg[9] = 0; */
        /* msg_in.msg[10] = 0; */
        /* msg_in.msg[11] = 0; */
        /* msg_in.msg[12] = 0; */
        /* msg_in.msg[13] = 0; */
        /* msg_in.msg[14] = 0; */
        /* msg_in.msg[15] = 0; */

        /* user_log_printf("ping sending %d, %d, %d, %d\n", */
        /*                 msg.tag, msg.msg[1], msg.msg[2], msg.msg[3]); */

        L4_MsgLoad(&msg_out);
        L4_Ipc(pong_id, ping_id, 0, (L4_ThreadId_t *)0);

        /* Response */
        L4_MsgClear(&msg_in);
        L4_MsgStore(ping_pong_res_tag, &msg_in);
        /* if (msg_in.msg[1] != 16 || msg_in.msg[2] != 17 || msg_in.msg[3] != 18 || msg_in.msg[4] != 19 || */
        /*     msg_in.msg[5] != 20 || msg_in.msg[6] != 21 || msg_in.msg[7] != 22 || msg_in.msg[8] != 23 || */
        /*     msg_in.msg[9] != 24 || msg_in.msg[10] != 25 || msg_in.msg[11] != 26 || msg_in.msg[12] != 27 || */
        /*     msg_in.msg[13] != 28 || msg_in.msg[14] != 29 || msg_in.msg[15] != 30) { */
        /*     user_log_puts("Ping got wrong data\n"); */
        /*     print_msg(&msg_in); */
        /* } */
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
    L4_Msg_t msg_out, msg_in;
    L4_Word_t msgs[16] = {
        16,
        17,
        18,
        19,
        20,
        21,
        22,
        23,
        24,
        25,
        26,
        27,
        28,
        29,
        30,
    };
    L4_MsgClear(&msg_out);
    L4_MsgPut(&msg_out, 0, 15, msgs, 0, NULL);
    ping_pong_res_tag = msg_out.tag;

    for (int i = 0; i < 100000; i++) {
        /* msg_in.msg[1] = 0; */
        /* msg_in.msg[2] = 0; */
        /* msg_in.msg[3] = 0; */
        /* msg_in.msg[4] = 0; */
        /* msg_in.msg[5] = 0; */
        /* msg_in.msg[6] = 0; */
        /* msg_in.msg[7] = 0; */
        /* msg_in.msg[8] = 0; */
        /* msg_in.msg[9] = 0; */
        /* msg_in.msg[10] = 0; */
        /* msg_in.msg[11] = 0; */
        /* msg_in.msg[12] = 0; */
        /* msg_in.msg[13] = 0; */
        /* msg_in.msg[14] = 0; */
        /* msg_in.msg[15] = 0; */

        /* Initial message */
        L4_Ipc(L4_nilthread, ping_id, 0, (L4_ThreadId_t *)0);
        L4_MsgStore(ping_pong_tag, &msg_in);
        /* if (msg_in.msg[1] != 1 || msg_in.msg[2] != 2 || msg_in.msg[3] != 3 || msg_in.msg[4] != 4 || */
        /*     msg_in.msg[5] != 5 || msg_in.msg[6] != 6 || msg_in.msg[7] != 7 || msg_in.msg[8] != 8 || */
        /*     msg_in.msg[9] != 9 || msg_in.msg[10] != 10 || msg_in.msg[11] != 11 || msg_in.msg[12] != 12 || */
        /*     msg_in.msg[13] != 13 || msg_in.msg[14] != 14 || msg_in.msg[15] != 15) { */
        /*     user_log_puts("Pong got wrong data\n"); */
        /*     print_msg(&msg_in); */
        /* } */
        /* user_log_printf("pong received %d, %d, %d, %d\n", */
        /*                 msg.tag, msg.msg[1], msg.msg[2], msg.msg[3]); */

        /* Response */

        /* user_log_printf("pong responding with %d, %d, %d, %d\n", */
        /*                 msg.tag, msg.msg[1], msg.msg[2], msg.msg[3]); */

        L4_MsgLoad(&msg_out);
        L4_Ipc(ping_id, L4_nilthread, 0, (L4_ThreadId_t *)0);
    }
    user_log_puts("pong done\n");

    while (1) {
        /* TODO: Need to use proper timeout object here */
        L4_Sleep(100000);
    }
}
