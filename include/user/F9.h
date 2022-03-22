#ifndef F9_H_
#define F9_H_

#include <stdint.h>
#include <types.h>
#include <kip.h>

typedef uint32_t L4_Word_t;
typedef L4_Word_t L4_ThreadId_t;
typedef void (*irq_handler_t)(void);

#define L4_nilthread  0UL
/* TODO: Use this instead #define L4_nilthread ((L4_ThreadId_t) { raw : 0UL}) */

void L4_Ipc(uint32_t to_gid, uint32_t from_gid);
void L4_Sleep();
void L4_ThreadControl(L4_ThreadId_t dest, L4_ThreadId_t SpaceSpecifier,
                      L4_ThreadId_t Scheduler, L4_ThreadId_t Pager,
                      void *UtcbLocation);
void L4_map(memptr_t base, uint32_t size, L4_ThreadId_t tid);

memptr_t get_free_base(kip_t *kip_ptr);
void map_user_sections(kip_t *kip_ptr, L4_ThreadId_t tid);
void request_irq(uint16_t irq_num, uint32_t priority, L4_ThreadId_t thread_id, uint32_t handler_ptr);

#endif // F9_H_
