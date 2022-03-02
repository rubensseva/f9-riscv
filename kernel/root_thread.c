#include "types.h"
#include <link.h>
#include <thread.h>
#include <syscall.h>
#include <ipc.h>
#include <uart_ESP32_C3.h>
#include <interrupt_ipc.h>
#include <l4/utcb.h>


__USER_DATA uint32_t uart_mem_base = 0x60000000;
__USER_DATA uint32_t uart_mem_size = 0xFFF;

__USER_DATA char txt[] = "hello world\n";

extern void* current_utcb;

typedef void (*irq_handler_t)(void);

void my_user_thread();
void root_thread(kip_t *kip_ptr, utcb_t *utcb_ptr);

typedef uint32_t L4_Word_t;
typedef L4_Word_t L4_ThreadId_t;

// TODO: Move to types.h or some other file
#define L4_nilthread  0UL
// TODO: Use this instead #define L4_nilthread ((L4_ThreadId_t) { raw : 0UL})

__USER_DATA L4_ThreadId_t root_id;
__USER_DATA L4_ThreadId_t user_id;

typedef struct {
    L4_Word_t base;
    L4_Word_t size;
} user_fpage_t;

struct user_struct {
    L4_Word_t tid;
    void (*entry)(void);
    L4_Word_t thread_num;
    user_fpage_t *fpages;
    const char *name;
};

struct user_struct my_user_thread_struct = {
  0,
  my_user_thread,
  0,
  NULL,
  "my-user-thread" };



memptr_t __USER_TEXT get_free_base(kip_t *kip_ptr)
{
  kip_mem_desc_t *desc = ((void *) kip_ptr) +
                         kip_ptr->memory_info.s.memory_desc_ptr;
  int n = kip_ptr->memory_info.s.n;
  int i = 0;

  for (i = 0; i < n; ++i) {
    // get the tag from size field (last 6 bits contains tag
    if ((desc[i].size & 0x3F) == 4)
      return desc[i].base & 0xFFFFFFC0;
  }

  return 0;
}

__USER_TEXT
void L4_Ipc(uint32_t to_gid, uint32_t from_gid) {
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

__USER_TEXT
void L4_Sleep() {
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


__USER_TEXT
void L4_ThreadControl(L4_ThreadId_t dest, L4_ThreadId_t SpaceSpecifier,
                      L4_ThreadId_t Scheduler, L4_ThreadId_t Pager,
                      void *UtcbLocation) {
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


/* void __USER_TEXT user_uart_handler() { */
/*   int c = uartgetc(); */
/*   if (c != -1) { */
/*     uartputc(c); */
/*   } */
/* } */

void __USER_TEXT L4_map(memptr_t base, uint32_t size, L4_ThreadId_t tid) {
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

void __USER_TEXT map_user_sections(kip_t *kip_ptr, L4_ThreadId_t tid)
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

void __USER_TEXT my_user_thread() {
  UART_write('n');
  /* Receive some data from root_thread */
  ipc_msg_tag_t tag = {{1, 0, 0, 0}};
  ((utcb_t *)current_utcb)->mr[0] = tag.raw;
  L4_Ipc(L4_nilthread, root_id);

  char *curr = txt;
  while (*curr != '\n') {
    UART_write(*curr);
    curr++;
  }

  /* Request UART interrupts */
  /* ipc_msg_tag_t irq_tag = {{0, 0, 0, 0}}; */
  /* irq_tag.s.n_untyped = 5; */
  /* irq_tag.s.label = USER_INTERRUPT_LABEL; */
  /* ((utcb_t *)current_utcb)->mr[0] = irq_tag.raw; */
  /* ((utcb_t *)current_utcb)->mr[1] = (uint16_t) 10; // IRQ_N */
  /* ((utcb_t *)current_utcb)->mr[2] = (l4_thread_t) user_id; */
  /* ((utcb_t *)current_utcb)->mr[3] = (uint16_t) USER_IRQ_ENABLE; // action */
  /* ((utcb_t *)current_utcb)->mr[4] = (void *) user_uart_handler; */
  /* ((utcb_t *)current_utcb)->mr[5] = (uint16_t) 1; // priority */
  /* L4_ThreadId_t irq_gid = TID_TO_GLOBALID(THREAD_IRQ_REQUEST); */
  /* L4_Ipc(irq_gid, L4_NILTHREAD); */

  /* while (1) { */
  /*   // Wait for IPC */
  /*   L4_ThreadId_t intr_tid = TID_TO_GLOBALID(THREAD_INTERRUPT); */
  /*   L4_Ipc(L4_nilthread, intr_tid); */
  /*   // At this point, the answer from IPC should be in the MRs */
  /*   // TODO: Not sure if user space should be able to view the utcb type? Maybe its ok? */
  /*   uint32_t *mrs = ((utcb_t*)current_utcb)->mr; */

  /*   ipc_msg_tag_t new_tag = {.raw = mrs[0]}; */
  /*   uint32_t irqn = mrs[IRQ_IPC_IRQN + 1]; */
  /*   irq_handler_t handler = mrs[IRQ_IPC_HANDLER + 1]; */
  /*   uint32_t action = mrs[IRQ_IPC_ACTION + 1]; */

  /*   switch (action) { */
  /*   case USER_IRQ_ENABLE: */
  /*     handler(); */
  /*     break; */
  /*     /\* case USER_IRQ_FREE: *\/ */
  /*     /\*    // return NULL; *\/ */
  /*     /\* } *\/ */
  /*   } */

  /*   // L4_Sleep(); */
  /* } */
  while (1) {}
}



/* kip_ptr and utcb_ptr will be passed through a0 and a1 by create_root_thread() */
void __USER_TEXT root_thread(kip_t *kip_ptr, utcb_t *utcb_ptr) {
    L4_ThreadId_t myself = utcb_ptr->t_globalid;
    root_id = myself;
    L4_ThreadId_t user_thread = TID_TO_GLOBALID(24);
    user_id = user_thread;

    /* Create user thread */
    char *free_mem = (char *) get_free_base(kip_ptr);
    /* Let threadid = spaceid, to tell the kernel to create a new address space, instead of sharing an existing one. */
    L4_ThreadControl(user_thread, user_thread, L4_nilthread, myself, free_mem);
    /* Give user thread all user sections */
    map_user_sections(kip_ptr, user_thread);
    /* Give user thread uart space */
    L4_map(uart_mem_base, uart_mem_size, user_thread);

    /* Start user thread */
    ipc_msg_tag_t tag = {{0, 0, 0, 0}};
    tag.s.n_untyped = 5;
    ((utcb_t *)current_utcb)->mr[0] = tag.raw;
    ((utcb_t *)current_utcb)->mr[1] = (uint32_t) my_user_thread; // pc
    ((utcb_t *)current_utcb)->mr[2] = (uint32_t) &user_thread_stack_end; // sp
    ((utcb_t *)current_utcb)->mr[3] = ((uint32_t) &user_thread_stack_end) - ((uint32_t) &user_thread_stack_start); // stack size
    ((utcb_t *)current_utcb)->mr[4] = 0;
    ((utcb_t *)current_utcb)->mr[5] = 0;
    L4_Ipc(user_thread, myself);

    /* Send some data to user thread */
    tag.s.n_untyped = 1;
    ((utcb_t *)current_utcb)->mr[0] = tag.raw;
    ((utcb_t *)current_utcb)->mr[1] = 1337;
    L4_Ipc(user_thread, myself);

    /* Sleep to allow user thread to be scheduled. */
    while (1) {
      L4_Sleep();
    }
}
