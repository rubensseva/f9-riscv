#include <stdint.h>
#include <types.h>
#include <link.h>
#include <thread.h>
#include <syscall.h>
#include <ipc.h>
#include <uart_ESP32_C3.h>
#include <interrupt_ipc.h>
#include <l4/utcb.h>
#include <user/F9.h>

extern void* current_utcb;

__USER_TEXT void L4_Ipc(uint32_t to_gid, uint32_t from_gid)
{
    __asm__ __volatile__(
        "mv a0, %0\n\t\
        mv a1, %1\n\t\
        mv a2, %2\n\t\
        mv a3, %3\n\t\
        ecall\n\t"
        :
        : "r"(SYS_IPC), "r"(to_gid), "r"(from_gid), "r"(0)
        : "a0", "a1", "a2", "a3");
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
    ipc_msg_tag_t tag = {.raw = 0};
    tag.s.n_typed = 2;

    /* 0xA is 0b1010, 0x8 is 0b1000, it decides what operation to do for typed argument:
       - 1xxx means that this is a grant or a map instead of a normal typed IPC.
       - xx1x means that this is a grant. */
    L4_Word_t page[2] = {
        (base & 0xFFFFFFC0) | 0xA,
        size & 0xFFFFFFC0
    };

    ((utcb_t *)current_utcb)->mr[0] = tag.raw;
    ((utcb_t *)current_utcb)->mr[1] = page[0];
    ((utcb_t *)current_utcb)->mr[2] = page[1];

    L4_Ipc(tid, L4_NILTHREAD);
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
        if (size > 0 && (tag == 2 || tag == 3)) { // tag 2 and 3 is user_text and user_data, from memory.h
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
    ipc_msg_tag_t irq_tag = {{0, 0, 0, 0}};
    irq_tag.s.n_untyped = 5;
    irq_tag.s.label = USER_INTERRUPT_LABEL;
    ((utcb_t *)current_utcb)->mr[0] = irq_tag.raw;
    ((utcb_t *)current_utcb)->mr[1] = (uint16_t) irq_num; // IRQ_N
    ((utcb_t *)current_utcb)->mr[2] = (l4_thread_t) thread_id;
    ((utcb_t *)current_utcb)->mr[3] = (uint16_t) USER_IRQ_ENABLE; // action
    ((utcb_t *)current_utcb)->mr[4] = (uint32_t) handler_ptr;
    ((utcb_t *)current_utcb)->mr[5] = (uint16_t) priority;
    L4_ThreadId_t irq_gid = TID_TO_GLOBALID(THREAD_IRQ_REQUEST);
    L4_Ipc(irq_gid, L4_NILTHREAD);
}
