#include <link.h>
#include <thread.h>
#include <syscall.h>
#include <ipc.h>

extern void* current_utcb;

void my_user_thread();
void root_thread(kip_t *kip_ptr, utcb_t *utcb_ptr);



typedef uint64_t L4_Word_t;
typedef L4_Word_t L4_ThreadId_t;

// TODO: Move to types.h or some other file
#define L4_nilthread  0UL
// TODO: Use this instead #define L4_nilthread ((L4_ThreadId_t) { raw : 0UL})

L4_ThreadId_t root_id;

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
void L4_Ipc(uint64_t to_gid, uint64_t from_gid) {
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

void __USER_TEXT my_user_thread() {
  L4_ThreadId_t gid = TID_TO_GLOBALID(24);

  ipc_msg_tag_t tag = {{1, 0, 0, 0}};
  ((utcb_t *)current_utcb)->mr[0] = tag.raw;
  L4_Ipc(L4_nilthread, root_id);

  while (1) {
    L4_Sleep();
  }
}

// kip_ptr and utcb_ptr will be passed through a0 and a1 by create_root_thread()
void __USER_TEXT root_thread(kip_t *kip_ptr, utcb_t *utcb_ptr) {
    L4_ThreadId_t myself = utcb_ptr->t_globalid;
    root_id = myself;
    char *free_mem = (char *) get_free_base(kip_ptr);

    L4_ThreadId_t gid = TID_TO_GLOBALID(24);

    // Create user thread
    L4_ThreadControl(gid, gid, L4_nilthread, myself, free_mem);

    ipc_msg_tag_t tag = {{5, 0, 0, 0}};

    // mr0 contains the tag
    // ((utcb_t *)current_utcb)->mr[0] = 5;
    ((utcb_t *)current_utcb)->mr[0] = tag.raw;
    ((utcb_t *)current_utcb)->mr[1] = (uint64_t) my_user_thread;
    ((utcb_t *)current_utcb)->mr[2] = (uint64_t) &user_thread_stack_end; // stack pointer
    ((utcb_t *)current_utcb)->mr[3] = ((uint64_t) &user_thread_stack_end) - ((uint64_t) &user_thread_stack_start);
    ((utcb_t *)current_utcb)->mr[4] = 0; // will be set to A2
    ((utcb_t *)current_utcb)->mr[5] = 0; // will be set to A3

    L4_Ipc(gid, myself);

    tag.s.n_untyped = 1;
    ((utcb_t *)current_utcb)->mr[0] = tag.raw;
    ((utcb_t *)current_utcb)->mr[1] = 1337;

    L4_Ipc(gid, myself);


    // Sleep to allow user thread to be scheduled.
    while (1) {
      L4_Sleep();
    }
}
