#include <types.h>
#include <link.h>
#include <thread.h>
#include <syscall.h>
#include <ipc.h>
#include <uart_ESP32_C3.h>
#include <interrupt_ipc.h>
#include <l4/utcb.h>
#include <user/F9.h>
#include <user/user_thread.h>


__USER_DATA uint32_t uart_mem_base = 0x60000000;
__USER_DATA uint32_t uart_mem_size = 0xFFF;

extern void* current_utcb;

/* Kip_ptr and utcb_ptr will be passed through a0 and a1 by create_root_thread() */
void __USER_TEXT root_thread(kip_t *kip_ptr, utcb_t *utcb_ptr)
{
    L4_ThreadId_t myself = utcb_ptr->t_globalid;
    L4_ThreadId_t user_thread_id = TID_TO_GLOBALID(24);

    /* Create user thread */
    char *free_mem = (char *) get_free_base(kip_ptr);
    /* Let threadid = spaceid, to tell the kernel to create a new address space, instead of sharing an existing one. */
    L4_ThreadControl(user_thread_id, user_thread_id, L4_nilthread, myself, free_mem);
    /* Give user thread all user sections */
    map_user_sections(kip_ptr, user_thread_id);
    /* Give user thread uart space */
    L4_map(uart_mem_base, uart_mem_size, user_thread_id);

    /* Start user thread */
    ipc_msg_tag_t tag = {{0, 0, 0, 0}};
    tag.s.n_untyped = 5;
    ((utcb_t *)current_utcb)->mr[0] = tag.raw;
    ((utcb_t *)current_utcb)->mr[1] = (uint32_t) user_thread; // pc
    ((utcb_t *)current_utcb)->mr[2] = (uint32_t) &user_thread_stack_end; // sp
    ((utcb_t *)current_utcb)->mr[3] = ((uint32_t) &user_thread_stack_end) - ((uint32_t) &user_thread_stack_start); // stack size
    ((utcb_t *)current_utcb)->mr[4] = 0;
    ((utcb_t *)current_utcb)->mr[5] = 0;
    L4_Ipc(user_thread_id, myself);

    /* Sleep to allow user thread to be scheduled. */
    while (1) {
        L4_Sleep();
    }
}
