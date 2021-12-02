#include <link.h>
#include <thread.h>
#include <syscall.h>

void my_user_thread();
void root_thread(kip_t *kip_ptr, utcb_t *utcb_ptr);

typedef uint64_t L4_Word_t;
typedef L4_Word_t L4_ThreadId_t;

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
    if ((desc[i].size & 0x3F) == 4)
      return desc[i].base & 0xFFFFFFC0;
  }

  return 0;
}


__USER_TEXT
void L4_ThreadControl(L4_ThreadId_t dest,
                           L4_ThreadId_t SpaceSpecifier,
                           L4_ThreadId_t Scheduler,
                           L4_ThreadId_t Pager,
                           void *UtcbLocation)
{
    __asm__ __volatile__(
        "mv a0, %0\n\t\
        mv a1, %1\n\t\
        mv a2, %2\n\t\
        mv a3, %3\n\t\
        mv a4, %4\n\t\
        ecall\n\t"
        :
        : "r"(SYS_THREAD_CONTROL), "r"(dest), "r"(SpaceSpecifier), "r"(Scheduler), "r"(Pager)
        : "a0", "a1", "a2", "a3");
}

void __USER_TEXT my_user_thread() {
    while(1);
}

void __USER_TEXT root_thread(kip_t *kip_ptr, utcb_t *utcb_ptr) {
	  char *free_mem = (char *) get_free_base(kip_ptr);
    __asm__ __volatile__(
        "mv a0, %0\n\t\
        mv a1, %1\n\t\
        mv a2, %2\n\t\
        add a3, x0, %3\n\t\
        ecall\n\t"
            :
            : "r"(SYS_THREAD_CONTROL), "r"(L4_NILTHREAD), "r"(L4_NILTHREAD), "r"(1000)
            : "a0", "a1", "a2", "a3");
  while (1);
}
