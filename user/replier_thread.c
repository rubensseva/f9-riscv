#include "replier_thread.h"
#include "include/message.h"

#include <platform/link.h>
#include <F9.h>
#include <user_types.h>
#include <user_thread_log.h>

__USER_DATA L4_ThreadId_t replier_id;

extern L4_ThreadId_t hoppus_thread_id;

void replier() {

    L4_MsgTag_t tag;
    user_log_printf("replier thread start, id: %d\n", replier_id);
    L4_Msg_t msg;
    L4_MsgClear(&msg);

    while (1) {
        /* Initial message */
        L4_Ipc(L4_nilthread, hoppus_thread_id, 0, (L4_ThreadId_t *)0);
        tag = (L4_MsgTag_t) __L4_MR0;
        L4_MsgStore(tag, &msg);
        user_log_puts("printer thread got an IPC message\n");
        user_log_printf("tag: %d untyped, %d typed\n", tag.X.u, tag.X.t);
        user_log_puts("msgs: ");
        for (int i = 0; i < tag.X.u; i++) {
            user_log_printf("%d ", msg.msg[i + 1]);
        }
        user_log_puts("\n");
    }
}
