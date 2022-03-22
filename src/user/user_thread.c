#include <types.h>
#include <user/F9.h>
#include <user/user_thread.h>
#include <uart_ESP32_C3.h>
#include <ipc.h>
#include <interrupt_ipc.h>
#include <interrupt.h>

extern void* current_utcb;

__USER_TEXT void user_uart_handler() {
    while(UART_rxfifo_count(0) != 0) {
        UART_write(UART_read(0), 0);
    }
    UART_clear(0);
}

__USER_TEXT void user_thread()
{
    L4_ThreadId_t myself = ((utcb_t *)current_utcb)->t_globalid;
    request_irq(3, 1, myself, (uint32_t) user_uart_handler);

    while (1) {
        L4_ThreadId_t intr_tid = TID_TO_GLOBALID(THREAD_INTERRUPT);
        L4_Ipc(L4_nilthread, intr_tid);
        uint32_t *mrs = ((utcb_t*)current_utcb)->mr;

        irq_handler_t handler = (irq_handler_t) mrs[IRQ_IPC_HANDLER + 1];
        uint32_t action = mrs[IRQ_IPC_ACTION + 1];

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
