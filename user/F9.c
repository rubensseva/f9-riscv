#include <message.h>
#include <F9.h>
#include <user_types.h>

extern void* current_utcb;

__USER_TEXT L4_MsgTag_t L4_Ipc(L4_ThreadId_t to,
                               L4_ThreadId_t FromSpecifier,
                               L4_Word_t Timeouts,
                               L4_ThreadId_t *from)
{
    L4_MsgTag_t result;
    L4_ThreadId_t from_ret;

    __asm__ __volatile__(
        "mv a0, %[SYS_NUM]\n\t\
        mv a1, %[to]\n\t\
        mv a2, %[from_specifier]\n\t\
        mv a3, %[timeout]\n\t\
        ecall\n\t\
        mv %[from_ret], a0\n\t"
        : [from_ret] "=r" (from_ret)
        : [SYS_NUM] "r"(SYS_IPC), [to] "r"(to), [from_specifier] "r"(FromSpecifier), [timeout] "r"(Timeouts)
        : "a0", "a1", "a2", "a3");

    result.raw = __L4_MR0;
    if (from)
        *from = from_ret;
    return result;
}

__USER_TEXT void L4_Sleep()
{
    __asm__ __volatile__(
        "mv a0, %0\n\t\
        mv a1, %1\n\t\
        mv a2, %2\n\t\
        mv a3, %3\n\t\
        ecall\n\t"
        :
        : "r"(SYS_IPC), "r"(L4_NILTHREAD), "r"(L4_NILTHREAD), "r"(100000)
        : "a0", "a1", "a2", "a3");
}


__USER_TEXT void L4_ThreadControl(L4_ThreadId_t dest, L4_ThreadId_t SpaceSpecifier,
                                  L4_ThreadId_t Scheduler, L4_ThreadId_t Pager,
                                  void *UtcbLocation)
{
    __asm__ __volatile__("mv a0, %0\n\t\
        mv a1, %1\n\t\
        mv a2, %2\n\t\
        mv a3, %3\n\t\
        mv a4, %4\n\t\
        mv a5, %5\n\t\
        ecall\n\t"
        :
        : "r"(SYS_THREAD_CONTROL), "r"(dest), "r"(SpaceSpecifier),
          "r"(Scheduler), "r"(Pager), "r"(UtcbLocation)
        : "a0", "a1", "a2", "a3", "a4", "a5");
}

__USER_TEXT void L4_map(memptr_t base, uint32_t size, L4_ThreadId_t tid)
{
    L4_Msg_t msg;

    /* 0xA is 0b1010, 0x8 is 0b1000, it decides what operation to do for typed argument:
       - 1xxx means that this is a grant or a map instead of a normal typed IPC.
       - xx1x means that this is a grant. */
    L4_Word_t page[2] = {
        (base & 0xFFFFFFC0) | 0xA,
        size & 0xFFFFFFC0
    };

    L4_MsgPut(&msg, 0, 0, NULL, 2, page);
    L4_MsgLoad(&msg);
    L4_Send(tid);
}

__USER_TEXT void map_user_sections(kip_t *kip_ptr, L4_ThreadId_t tid)
{
    kip_mem_desc_t *desc = ((void *) kip_ptr) +
        kip_ptr->memory_info.s.memory_desc_ptr;
    int n = kip_ptr->memory_info.s.n;
    int i = 0;

    for (i = 0; i < n; ++i) {
        uint32_t tag = desc[i].size & 0x3F;
        uint32_t size = desc[i].size & ~0x3F;
        /* tag 2 and 3 is user_text and user_data, from memory.h */
        if (size > 0 && (tag == 2 || tag == 3)) {
            L4_map(desc[i].base, desc[i].size, tid);
        }
    }
}

__USER_TEXT memptr_t get_free_base(kip_t *kip_ptr)
{
    kip_mem_desc_t *desc = ((void *) kip_ptr) +
        kip_ptr->memory_info.s.memory_desc_ptr;
    int n = kip_ptr->memory_info.s.n;
    int i = 0;

    for (i = 0; i < n; ++i) {
        /* get the tag from size field (last 6 bits contains tag */
        if ((desc[i].size & 0x3F) == 4)
            return desc[i].base & 0xFFFFFFC0;
    }

    return 0;
}

__USER_TEXT void request_irq(uint16_t irq_num, uint32_t priority, L4_ThreadId_t thread_id, uint32_t handler_ptr) {
    L4_ThreadId_t irq_gid = {.raw = TID_TO_GLOBALID(THREAD_IRQ_REQUEST)};
    L4_Msg_t msg;
    L4_MsgClear(&msg);
    L4_Word_t msgs[5] = {
        (uint16_t) irq_num, // IRQ_N
        thread_id.raw,
        (uint16_t) USER_IRQ_ENABLE, // action
        (uint32_t) handler_ptr,
        (uint16_t) priority,
    };
    L4_MsgPut(&msg, USER_INTERRUPT_LABEL, 5, msgs, 0, NULL);
    L4_MsgLoad(&msg);
    L4_Ipc(irq_gid, L4_nilthread, 0, (L4_ThreadId_t *)0);
}
