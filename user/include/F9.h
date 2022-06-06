#ifndef F9_H_
#define F9_H_

#include <stdint.h>
#include <types.h>
#include <kip.h>
#include <platform/link.h>
#include <thread.h>
#include <syscall.h>
#include <ipc.h>
#include <platform/ESP32_C3_UART.h>
#include <interrupt_ipc.h>
#include <l4/utcb.h>

#include <message.h>
#include <user_types.h>
#include <vregs.h>

// void L4_Ipc(L4_ThreadId_t to_gid, L4_ThreadId_t from_gid);
L4_MsgTag_t L4_Ipc(L4_ThreadId_t to,
                   L4_ThreadId_t FromSpecifier,
                   L4_Word_t Timeouts,
                   L4_ThreadId_t *from);

void L4_Sleep();
void L4_ThreadControl(L4_ThreadId_t dest, L4_ThreadId_t SpaceSpecifier,
                      L4_ThreadId_t Scheduler, L4_ThreadId_t Pager,
                      void *UtcbLocation);
void L4_map(memptr_t base, uint32_t size, L4_ThreadId_t tid);

memptr_t get_free_base(kip_t *kip_ptr);
void map_user_text(kip_t *kip_ptr, L4_ThreadId_t tid);
void request_irq(uint16_t irq_num, uint32_t priority, L4_ThreadId_t thread_id, uint32_t handler_ptr);


L4_INLINE L4_MsgTag_t L4_Send_Timeout(L4_ThreadId_t to, L4_Time_t SndTimeout)
{
    return L4_Ipc (to, L4_nilthread,
            (L4_Word_t) SndTimeout.raw,
            (L4_ThreadId_t *) 0);
}

L4_INLINE L4_MsgTag_t L4_Send(L4_ThreadId_t to)
{
    return L4_Send_Timeout(to, L4_Never);
}

#endif // F9_H_
