#include <message.h>
#include <user_types.h>
#include <types.h>
#include <F9.h>
#include <user_thread.h>
#include <uart_ESP32_C3.h>
#include <ipc.h>
#include <interrupt_ipc.h>
#include <interrupt.h>
#include <malloc.h>

extern void* current_utcb;

__USER_TEXT void user_uart_handler() {
    while(UART_rxfifo_count(0) != 0) {
        UART_write(UART_read(0), 0);
    }
    UART_clear(0);
}

__USER_TEXT void user_thread()
{
    UART_receive_init(0);
    void *mem_test = malloc(124);
    void *mem_test2 = malloc(4);
    void *mem_test3 = malloc(300);
    free(mem_test2);
    void *mem_test4 = malloc(2);

    L4_ThreadId_t myself = {.raw = ((utcb_t *)current_utcb)->t_globalid};
    request_irq(3, 1, myself, (uint32_t) user_uart_handler);


    while (1) {
        UART_clear(0);
        L4_ThreadId_t intr_tid = {.raw = TID_TO_GLOBALID(THREAD_INTERRUPT)};
        L4_Ipc(L4_nilthread, intr_tid, 0, (L4_ThreadId_t *)0);

        L4_Msg_t msg;
        L4_MsgClear(&msg);
        msg.tag.X.label = USER_INTERRUPT_LABEL;
        msg.tag.X.u = IRQ_IPC_MSG_NUM;

        L4_MsgStore(msg.tag, &msg);

        irq_handler_t handler = (irq_handler_t) msg.msg[IRQ_IPC_HANDLER + 1];
        uint32_t action = msg.msg[IRQ_IPC_ACTION + 1];

        switch (action) {
            case USER_IRQ_ENABLE:
                handler();
                break;
                /* case USER_IRQ_FREE: */
                /*    return NULL; */
                /* } */
        }
    }
    while (1) {}
}
