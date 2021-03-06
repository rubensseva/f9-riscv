#include <message.h>
#include <user_types.h>
#include <types.h>
#include <F9.h>
#include <user_thread.h>
#include <platform/ESP32_C3_UART.h>
#include <ESP32_C3_timer.h>
#include <ipc.h>
#include <interrupt_ipc.h>
#include <interrupt.h>

#include <hoppus_entry.h>


#define UART_IRQN 3

extern void* current_utcb;

__USER_TEXT void user_uart_handler() {
    while(UART_rxfifo_count(0) != 0) {
        UART_write(UART_read(0), 0);
    }
    UART_clear(0);
}

__USER_TEXT int read_line(char *buf) {
    uint32_t count = 0;
    while (1) {
        UART_clear(0);
        UART_receive_en(0);
        /* Flush read buffer */
        while (UART_rxfifo_count(0) != 0) {
            char c = UART_read(0);
        }
        L4_ThreadId_t intr_tid = {.raw = TID_TO_GLOBALID(THREAD_INTERRUPT)};
        L4_Ipc(L4_nilthread, intr_tid, 0, (L4_ThreadId_t *)0);
        UART_receive_dis(0);

        L4_Msg_t msg;
        L4_MsgClear(&msg);
        msg.tag.X.label = USER_INTERRUPT_LABEL;
        msg.tag.X.u = IRQ_IPC_MSG_NUM;

        L4_MsgStore(msg.tag, &msg);

        // irq_handler_t handler = (irq_handler_t) msg.msg[IRQ_IPC_HANDLER + 1];
        // uint32_t action = msg.msg[IRQ_IPC_ACTION + 1];
        uint32_t irqn = msg.msg[IRQ_IPC_IRQN + 1];


        if (irqn == UART_IRQN) {
            while (UART_rxfifo_count(0) != 0) {
                char c = UART_read(0);
                UART_write(c, 0);
                buf[count++] = c;
                if (c == '\n') {
                    buf[count++] = '\0';
                    return 0;
                }
            }
            /* UART_clear(0); */
        }
    }
}


__USER_TEXT void user_thread()
{
    UART_receive_init(0);
    UART_receive_en(0);

    timer_init();

    L4_ThreadId_t myself = {.raw = ((utcb_t *)current_utcb)->t_globalid};
    request_irq(UART_IRQN, 1, myself, (uint32_t) user_uart_handler);

    clisp_main();

    while (1) {};
}
