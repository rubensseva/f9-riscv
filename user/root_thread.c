#include <types.h>
#include <link.h>
#include <thread.h>
#include <syscall.h>
#include <ipc.h>
#include <uart_ESP32_C3.h>
#include <interrupt_ipc.h>
#include <l4/utcb.h>
#include <F9.h>
#include <user_thread.h>
#include <message.h>


__USER_DATA uint32_t uart_mem_base = 0x60000000;
__USER_DATA uint32_t uart_mem_size = 0xFFF;

extern void* current_utcb;

/* Kip_ptr and utcb_ptr will be passed through a0 and a1 by create_root_thread() */
void __USER_TEXT root_thread(kip_t *kip_ptr, utcb_t *utcb_ptr)
{
    L4_ThreadId_t myself = {.raw = utcb_ptr->t_globalid};
    L4_ThreadId_t user_thread_id = {.raw = TID_TO_GLOBALID(24)};

    /* Create user thread */
    char *free_mem = (char *) get_free_base(kip_ptr);
    /* Let threadid = spaceid, to tell the kernel to create a new address space, instead of sharing an existing one. */
    L4_ThreadControl(user_thread_id, user_thread_id, L4_nilthread, myself, free_mem);

    /* Give user thread all user text */
    map_user_text(kip_ptr, user_thread_id);

    /* Give user threads all other required regions */
    L4_map((uint32_t)&user_thread_stack_start,
           (char *)&user_thread_stack_end - (char *)&user_thread_stack_start,
           user_thread_id);
    L4_map((uint32_t)&user_thread_heap_start,
           (char *)&user_thread_heap_end - (char *)&user_thread_heap_start,
           user_thread_id);
    L4_map((uint32_t)&user_data_misc_start,
           (char *)&user_data_misc_end - (char *)&user_data_misc_start,
           user_thread_id);
    L4_map(uart_mem_base, uart_mem_size, user_thread_id);

    /* Start user thread */
    L4_Msg_t msg;
    L4_MsgClear(&msg);

    L4_Word_t msgs[5] = {
        (L4_Word_t) user_thread,
        (L4_Word_t) &user_thread_stack_end,
        (L4_Word_t)(((uint32_t) &user_thread_stack_end) - ((uint32_t) &user_thread_stack_start)), // stack size
        0,
        0
    };

    L4_MsgPut(&msg, 0, 5, msgs, 0, NULL);
    L4_MsgLoad(&msg);
    L4_Ipc(user_thread_id, myself, 0, (L4_ThreadId_t *)0);

    while (1) {
        L4_Sleep();
    }
}
