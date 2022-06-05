#include <types.h>
#include <link.h>
#include <thread.h>
#include <syscall.h>
#include <ipc.h>
#include <uart_ESP32_C3.h>
#include <timer_ESP32_C3.h>
#include <interrupt_ipc.h>
#include <l4/utcb.h>
#include <F9.h>
#include <user_thread.h>
#include <message.h>
#include <user_stdio.h>
#include <ping_pong.h>


__USER_DATA uint32_t uart_mem_base = 0x60000000;
__USER_DATA uint32_t uart_mem_size = 0xFFF;
__USER_DATA uint32_t timer_mem_base = 0x6001F000;
__USER_DATA uint32_t timer_mem_size = 0xFFF;

extern void* current_utcb;

/* Kip_ptr and utcb_ptr will be passed through a0 and a1 by create_root_thread() */
void __USER_TEXT root_thread(kip_t *kip_ptr, utcb_t *utcb_ptr)
{

    L4_ThreadId_t myself = {.raw = utcb_ptr->t_globalid};
    L4_ThreadId_t user_thread_id = {.raw = TID_TO_GLOBALID(24)};



    char *free_mem = (char *) get_free_base(kip_ptr);



    /***** Hoppus thread *****/
    /* Create user thread */
    /* Let threadid = spaceid, to tell the kernel to create a new address space, instead of sharing an existing one. */
    L4_ThreadControl(user_thread_id, user_thread_id, L4_nilthread, myself, free_mem);

    /* Give user thread all user text */
    map_user_text(kip_ptr, user_thread_id);

    /* Give user threads all other required regions */
    L4_map((uint32_t)&hoppus_thread_stack_start,
           (char *)&hoppus_thread_stack_end - (char *)&hoppus_thread_stack_start,
           user_thread_id);
    L4_map((uint32_t)&hoppus_thread_heap_start,
           (char *)&hoppus_thread_heap_end - (char *)&hoppus_thread_heap_start,
           user_thread_id);
    L4_map((uint32_t)&user_threads_data_start,
           (char *)&user_threads_data_end - (char *)&user_threads_data_start,
           user_thread_id);
    L4_map(uart_mem_base, uart_mem_size, user_thread_id);
    L4_map(timer_mem_base, timer_mem_size, user_thread_id);

    /* Start user thread */
    L4_Msg_t msg;
    L4_MsgClear(&msg);

    L4_Word_t msgs[5] = {
        (L4_Word_t) user_thread,
        (L4_Word_t) &hoppus_thread_stack_end,
        (L4_Word_t)(((uint32_t) &hoppus_thread_stack_end) - ((uint32_t) &hoppus_thread_stack_start)), // stack size
        0,
        0
    };

    L4_MsgPut(&msg, 0, 5, msgs, 0, NULL);
    L4_MsgLoad(&msg);
    L4_Ipc(user_thread_id, myself, 0, (L4_ThreadId_t *)0);




    /***** Ping thread *****/
    /* { */
    /*     ping_id = (L4_ThreadId_t){.raw = TID_TO_GLOBALID(50)}; */


    /*     L4_ThreadControl(ping_id, ping_id, L4_nilthread, myself, free_mem); */

    /*     map_user_text(kip_ptr, ping_id); */

    /*     L4_map((uint32_t)&ping_thread_stack_start, */
    /*         (char *)&ping_thread_stack_end - (char *)&ping_thread_stack_start, */
    /*         ping_id); */
    /*     L4_map((uint32_t)&user_threads_data_start, */
    /*         (char *)&user_threads_data_end - (char *)&user_threads_data_start, */
    /*         ping_id); */

    /*     L4_Msg_t msg; */
    /*     L4_MsgClear(&msg); */
    /*     L4_Word_t msgs[5] = { */
    /*         (L4_Word_t) ping, */
    /*         (L4_Word_t) &ping_thread_stack_end, */
    /*         (L4_Word_t)(((uint32_t) &ping_thread_stack_end) - ((uint32_t) &ping_thread_stack_start)), // stack size */
    /*         0, */
    /*         0 */
    /*     }; */

    /*     L4_MsgPut(&msg, 0, 5, msgs, 0, NULL); */
    /*     L4_MsgLoad(&msg); */
    /*     L4_Ipc(ping_id, myself, 0, (L4_ThreadId_t *)0); */
    /* } */



    /***** Pong thread *****/
    /* { */
    /*     pong_id = (L4_ThreadId_t){.raw = TID_TO_GLOBALID(51)}; */

    /*     L4_ThreadControl(pong_id, pong_id, L4_nilthread, myself, free_mem); */

    /*     map_user_text(kip_ptr, pong_id); */

    /*     L4_map((uint32_t)&pong_thread_stack_start, */
    /*         (char *)&pong_thread_stack_end - (char *)&pong_thread_stack_start, */
    /*         pong_id); */
    /*     L4_map((uint32_t)&user_threads_data_start, */
    /*            (char *)&user_threads_data_end - (char *)&user_threads_data_start, */
    /*         pong_id); */

    /*     L4_Msg_t msg; */
    /*     L4_MsgClear(&msg); */

    /*     L4_Word_t msgs[5] = { */
    /*         (L4_Word_t) pong, */
    /*         (L4_Word_t) &pong_thread_stack_end, */
    /*         (L4_Word_t)(((uint32_t) &pong_thread_stack_end) - ((uint32_t) &pong_thread_stack_start)), // stack size */
    /*         0, */
    /*         0 */
    /*     }; */

    /*     L4_MsgPut(&msg, 0, 5, msgs, 0, NULL); */
    /*     L4_MsgLoad(&msg); */
    /*     L4_Ipc(pong_id, myself, 0, (L4_ThreadId_t *)0); */
    /* } */




    /* ipc_time_t sleep = {.raw = 0}; */
    /* sleep.period.m = 500; */

    /* sleep.period.e = 5; */
    /* user_printf("root sleeping for %d\n", (sleep.period.m << sleep.period.e)); */
    /* timer_init(); */
    /* TIMER_START(); */
    /* L4_Sleep(sleep); */
    /* TIMER_LATCH(); */
    /* int res = timer_get(); */
    /* user_puts("done sleeping\n"); */
    /* user_printf("timed sleep: %d\n", res); */
    /* int useconds = timer_counter_to_microseconds(res); */
    /* user_printf("timed sleep us: %dus\n", useconds); */

    /* sleep.period.e = 4; */
    /* user_printf("root sleeping for %d\n", (sleep.period.m << sleep.period.e)); */
    /* timer_reset(); */
    /* TIMER_START(); */
    /* L4_Sleep(sleep); */
    /* TIMER_LATCH(); */
    /* res = timer_get(); */
    /* user_puts("done sleeping\n"); */
    /* user_printf("timed sleep: %d\n", res); */
    /* useconds = timer_counter_to_microseconds(res); */
    /* user_printf("timed sleep us: %dus\n", useconds); */

    while (1) {
        /* TODO: Need to use proper timeout object here */
        L4_Sleep(100000);
    }
}
