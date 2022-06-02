#include <user_log.h>
#include <link.h>
#include <message.h>
#include <F9.h>
#include <user_stdarg.h>

int __USER_TEXT __user_log_printf(const char *format, ...)
{
    L4_Msg_t msg;
    va_list va;

    va_start(va, format);

    L4_MsgClear(&msg);
    L4_MsgAppendWord(&msg, (L4_Word_t)format);
    L4_MsgAppendWord(&msg, (L4_Word_t)&va);

    L4_MsgLoad(&msg);
    L4_Send((L4_ThreadId_t) {
        .raw = TID_TO_GLOBALID(THREAD_LOG)
    });

    va_end(va);

    return 0;
}
