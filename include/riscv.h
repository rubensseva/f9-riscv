#include <stdint.h>

static inline void wait_for_interrupt(void)
{
  __asm__ __volatile__ ("wfi");
}

/* privilege levels encodings
   privileged spec, 20190608, page 3 */
#define U_MODE_LEVEL 0
#define S_MODE_LEVEL 1
#define M_MODE_LEVEL 3

/* machine Status Register, mstatus */
#define MSTATUS_MPP_MASK (3L << 11) // previous mode.
#define MSTATUS_MPP_M (3L << 11)
#define MSTATUS_MPP_S (1L << 11)
#define MSTATUS_MPP_U (0L << 11)
#define MSTATUS_MIE (1L << 3)    // machine-mode interrupt enable.
#define MSTATUS_SIE (1L << 1)    // supervisor-mode interrupt enable
#define MSTATUS_UIE (1L << 0)    // user-mode interrupt enable
#define MSTATUS_MPIE (1L << 7)   // machine-mode previous interrupt enable

static inline uint32_t r_mstatus()
{
  uint32_t x;
  asm volatile("csrr %0, mstatus" : "=r" (x) );
  return x;
}

static inline void w_mstatus(uint32_t x)
{
  asm volatile("csrw mstatus, %0" : : "r" (x));
}

/* machine exception program counter, holds the
   instruction address to which a return from
   exception will go. */
static inline void w_mepc(uint32_t x)
{
  asm volatile("csrw mepc, %0" : : "r" (x));
}

static inline uint32_t r_mepc()
{
  uint32_t x;
  asm volatile("csrr %0, mepc" : "=r" (x));
  return x;
}

static inline uint32_t r_mtval()
{
  uint32_t x;
  asm volatile("csrr %0, mtval" : "=r" (x));
  return x;
}

// Supervisor Status Register, sstatus

#define SSTATUS_SPP (1L << 8)  // Previous mode, 1=Supervisor, 0=User
#define SSTATUS_SPIE (1L << 5) // Supervisor Previous Interrupt Enable
#define SSTATUS_UPIE (1L << 4) // User Previous Interrupt Enable
#define SSTATUS_SIE (1L << 1)  // Supervisor Interrupt Enable
#define SSTATUS_UIE (1L << 0)  // User Interrupt Enable


static inline uint32_t r_sstatus()
{
  uint32_t x;
  asm volatile("csrr %0, sstatus" : "=r" (x) );
  return x;
}

static inline void w_sstatus(uint32_t x)
{
  asm volatile("csrw sstatus, %0" : : "r" (x));
}

// Supervisor Interrupt Pending
static inline uint32_t r_sip()
{
  uint32_t x;
  asm volatile("csrr %0, sip" : "=r" (x) );
  return x;
}

static inline void w_sip(uint32_t x)
{
  asm volatile("csrw sip, %0" : : "r" (x));
}

// Supervisor Interrupt Enable
#define SIE_SEIE (1L << 9) // external
#define SIE_STIE (1L << 5) // timer
#define SIE_SSIE (1L << 1) // software
static inline uint32_t r_sie()
{
  uint32_t x;
  asm volatile("csrr %0, sie" : "=r" (x) );
  return x;
}

static inline void w_sie(uint32_t x)
{
  asm volatile("csrw sie, %0" : : "r" (x));
}

// Machine-mode Interrupt Enable
#define MIE_MEIE (1L << 11) // m-mode external
#define MIE_MTIE (1L << 7)  // m-mode timer
#define MIE_MSIE (1L << 3)  // m-mode software
#define MIE_USIE (1L << 0)  // u-mode software interrupts
#define MIE_UEIE (1L << 8)  // u-mode external interrupts
static inline uint32_t r_mie()
{
  uint32_t x;
  asm volatile("csrr %0, mie" : "=r" (x) );
  return x;
}

static inline void w_mie(uint32_t x)
{
  asm volatile("csrw mie, %0" : : "r" (x));
}

// supervisor exception program counter, holds the
// instruction address to which a return from
// exception will go.
static inline void w_sepc(uint32_t x)
{
  asm volatile("csrw sepc, %0" : : "r" (x));
}

static inline uint32_t r_sepc()
{
  uint32_t x;
  asm volatile("csrr %0, sepc" : "=r" (x) );
  return x;
}

// Machine Exception Delegation
static inline uint32_t r_medeleg()
{
  uint32_t x;
  asm volatile("csrr %0, medeleg" : "=r" (x) );
  return x;
}

static inline void w_medeleg(uint32_t x)
{
  asm volatile("csrw medeleg, %0" : : "r" (x));
}

// Machine Interrupt Delegation
static inline uint32_t r_mideleg()
{
  uint32_t x;
  asm volatile("csrr %0, mideleg" : "=r" (x) );
  return x;
}

static inline void w_mideleg(uint32_t x)
{
  asm volatile("csrw mideleg, %0" : : "r" (x));
}

// Supervisor Trap-Vector Base Address
// low two bits are mode.
static inline void w_stvec(uint32_t x)
{
  asm volatile("csrw stvec, %0" : : "r" (x));
}

static inline uint32_t r_stvec()
{
  uint32_t x;
  asm volatile("csrr %0, stvec" : "=r" (x) );
  return x;
}

// Machine-mode interrupt vector
static inline void w_mtvec(uint32_t x)
{
  asm volatile("csrw mtvec, %0" : : "r" (x));
}

static inline void w_pmpcfg0(uint32_t x)
{
  asm volatile("csrw pmpcfg0, %0" : : "r" (x));
}
static inline void w_pmpcfg1(uint32_t x)
{
  asm volatile("csrw pmpcfg1, %0" : : "r" (x));
}
static inline void w_pmpcfg2(uint32_t x)
{
  asm volatile("csrw pmpcfg2, %0" : : "r" (x));
}
static inline void w_pmpcfg3(uint32_t x)
{
  asm volatile("csrw pmpcfg3, %0" : : "r" (x));
}
static inline uint32_t r_pmpcfg0()
{
  uint32_t x;
  asm volatile("csrr %0, pmpcfg0" : "=r" (x) );
  return x;
}
static inline uint32_t r_pmpcfg1()
{
  uint32_t x;
  asm volatile("csrr %0, pmpcfg1" : "=r" (x) );
  return x;
}
static inline uint32_t r_pmpcfg2()
{
  uint32_t x;
  asm volatile("csrr %0, pmpcfg2" : "=r" (x) );
  return x;
}
static inline uint32_t r_pmpcfg3()
{
  uint32_t x;
  asm volatile("csrr %0, pmpcfg3" : "=r" (x) );
  return x;
}

static inline void w_pmpaddr0(uint32_t x)
{
  asm volatile("csrw pmpaddr0, %0" : : "r" (x));
}
static inline void w_pmpaddr1(uint32_t x)
{
  asm volatile("csrw pmpaddr1, %0" : : "r" (x));
}
static inline void w_pmpaddr2(uint32_t x)
{
  asm volatile("csrw pmpaddr2, %0" : : "r" (x));
}
static inline void w_pmpaddr3(uint32_t x)
{
  asm volatile("csrw pmpaddr3, %0" : : "r" (x));
}
static inline void w_pmpaddr4(uint32_t x)
{
  asm volatile("csrw pmpaddr4, %0" : : "r" (x));
}
static inline void w_pmpaddr5(uint32_t x)
{
  asm volatile("csrw pmpaddr5, %0" : : "r" (x));
}
static inline void w_pmpaddr6(uint32_t x)
{
  asm volatile("csrw pmpaddr6, %0" : : "r" (x));
}
static inline void w_pmpaddr7(uint32_t x)
{
  asm volatile("csrw pmpaddr7, %0" : : "r" (x));
}
static inline void w_pmpaddr8(uint32_t x)
{
  asm volatile("csrw pmpaddr8, %0" : : "r" (x));
}
static inline void w_pmpaddr9(uint32_t x)
{
  asm volatile("csrw pmpaddr9, %0" : : "r" (x));
}
static inline void w_pmpaddr10(uint32_t x)
{
  asm volatile("csrw pmpaddr10, %0" : : "r" (x));
}
static inline void w_pmpaddr11(uint32_t x)
{
  asm volatile("csrw pmpaddr11, %0" : : "r" (x));
}
static inline void w_pmpaddr12(uint32_t x)
{
  asm volatile("csrw pmpaddr12, %0" : : "r" (x));
}
static inline void w_pmpaddr13(uint32_t x)
{
  asm volatile("csrw pmpaddr13, %0" : : "r" (x));
}
static inline void w_pmpaddr14(uint32_t x)
{
  asm volatile("csrw pmpaddr14, %0" : : "r" (x));
}
static inline void w_pmpaddr15(uint32_t x)
{
  asm volatile("csrw pmpaddr15, %0" : : "r" (x));
}


static inline uint32_t r_pmpaddr0() {
  uint32_t x;
  asm volatile("csrr %0, pmpaddr0" : "=r" (x) );
  return x;
}
static inline uint32_t r_pmpaddr1() {
  uint32_t x;
  asm volatile("csrr %0, pmpaddr1" : "=r" (x));
  return x;
}
static inline uint32_t r_pmpaddr2() {
  uint32_t x;
  asm volatile("csrr %0, pmpaddr2" : "=r" (x));
  return x;
}
static inline uint32_t r_pmpaddr3() {
  uint32_t x;
  asm volatile("csrr %0, pmpaddr3" : "=r" (x));
  return x;
}
static inline uint32_t r_pmpaddr4() {
  uint32_t x;
  asm volatile("csrr %0, pmpaddr4" : "=r" (x));
  return x;
}
static inline uint32_t r_pmpaddr5() {
  uint32_t x;
  asm volatile("csrr %0, pmpaddr5" : "=r" (x));
  return x;
}
static inline uint32_t r_pmpaddr6() {
  uint32_t x;
  asm volatile("csrr %0, pmpaddr6" : "=r" (x));
  return x;
}
static inline uint32_t r_pmpaddr7() {
  uint32_t x;
  asm volatile("csrr %0, pmpaddr7" : "=r" (x));
  return x;
}
static inline uint32_t r_pmpaddr8() {
  uint32_t x;
  asm volatile("csrr %0, pmpaddr8" : "=r" (x));
  return x;
}
static inline uint32_t r_pmpaddr9() {
  uint32_t x;
  asm volatile("csrr %0, pmpaddr9" : "=r" (x));
  return x;
}
static inline uint32_t r_pmpaddr10() {
  uint32_t x;
  asm volatile("csrr %0, pmpaddr10" : "=r" (x));
  return x;
}
static inline uint32_t r_pmpaddr11() {
  uint32_t x;
  asm volatile("csrr %0, pmpaddr11" : "=r" (x));
  return x;
}
static inline uint32_t r_pmpaddr12() {
  uint32_t x;
  asm volatile("csrr %0, pmpaddr12" : "=r" (x));
  return x;
}
static inline uint32_t r_pmpaddr13() {
  uint32_t x;
  asm volatile("csrr %0, pmpaddr13" : "=r" (x));
  return x;
}
static inline uint32_t r_pmpaddr14() {
  uint32_t x;
  asm volatile("csrr %0, pmpaddr14" : "=r" (x));
  return x;
}
static inline uint32_t r_pmpaddr15() {
  uint32_t x;
  asm volatile("csrr %0, pmpaddr15" : "=r" (x));
  return x;
}

// use riscv's sv39 page table scheme.
#define SATP_SV39 (8L << 60)

#define MAKE_SATP(pagetable) (SATP_SV39 | (((uint32_t)pagetable) >> 12))

// supervisor address translation and protection;
// holds the address of the page table.
static inline void w_satp(uint32_t x)
{
  asm volatile("csrw satp, %0" : : "r" (x));
}

static inline uint32_t r_satp()
{
  uint32_t x;
  asm volatile("csrr %0, satp" : "=r" (x) );
  return x;
}

// Supervisor Scratch register, for early trap handler in trampoline.S.
static inline void w_sscratch(uint32_t x)
{
  asm volatile("csrw sscratch, %0" : : "r" (x));
}

static inline void w_mscratch(uint32_t x)
{
  asm volatile("csrw mscratch, %0" : : "r" (x));
}

// Supervisor Trap Cause
static inline uint32_t r_scause()
{
  uint32_t x;
  asm volatile("csrr %0, scause" : "=r" (x) );
  return x;
}

// Supervisor Trap Cause
static inline uint32_t r_mcause()
{
  uint32_t x;
  asm volatile("csrr %0, mcause" : "=r" (x) );
  return x;
}

// Supervisor Trap Value
static inline uint32_t r_stval()
{
  uint32_t x;
  asm volatile("csrr %0, stval" : "=r" (x) );
  return x;
}

// Machine-mode Counter-Enable
static inline void  w_mcounteren(uint32_t x)
{
  asm volatile("csrw mcounteren, %0" : : "r" (x));
}

static inline uint32_t r_mcounteren()
{
  uint32_t x;
  asm volatile("csrr %0, mcounteren" : "=r" (x) );
  return x;
}

// machine-mode cycle counter
static inline uint32_t r_time()
{
  uint32_t x;
  asm volatile("csrr %0, time" : "=r" (x) );
  return x;
}

/* requires machine-mode */
static inline void interrupt_enable() {
	w_mstatus(r_mstatus() | (MSTATUS_MIE | MSTATUS_UIE));
}

/* requires machine-mode */
static inline void interrupt_disable() {
	w_mstatus(r_mstatus() & ~(MSTATUS_MIE | MSTATUS_UIE));
}

// are device interrupts enabled?
static inline int intr_get()
{
  uint32_t x = r_sstatus();
  return (x & SSTATUS_SIE) != 0;
}

static inline uint32_t r_sp()
{
  uint32_t x;
  asm volatile("mv %0, sp" : "=r" (x) );
  return x;
}


// read and write tp, the thread pointer, which holds
// this core's hartid (core number), the index into cpus[].
static inline uint32_t r_tp()
{
  uint32_t x;
  asm volatile("mv %0, tp" : "=r" (x) );
  return x;
}

static inline void w_tp(uint32_t x)
{
  asm volatile("mv tp, %0" : : "r" (x));
}

static inline uint32_t r_ra()
{
  uint32_t x;
  asm volatile("mv %0, ra" : "=r" (x) );
  return x;
}

// flush the TLB.
static inline void sfence_vma()
{
  // the zero, zero means flush all TLB entries.
  asm volatile("sfence.vma zero, zero");
}

