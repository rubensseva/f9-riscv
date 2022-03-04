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


/* Machine-mode Interrupt Enable */
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

static inline void w_mtvec(uint32_t x)
{
    asm volatile("csrw mtvec, %0" : : "r" (x));
}

static inline uint32_t r_satp()
{
    uint32_t x;
    asm volatile("csrr %0, satp" : "=r" (x) );
    return x;
}

/* Machine-mode interrupt vector */
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

static inline uint32_t r_pmpaddr0()
{
    uint32_t x;
    asm volatile("csrr %0, pmpaddr0" : "=r" (x) );
    return x;
}
static inline uint32_t r_pmpaddr1()
{
    uint32_t x;
    asm volatile("csrr %0, pmpaddr1" : "=r" (x));
    return x;
}
static inline uint32_t r_pmpaddr2()
{
    uint32_t x;
    asm volatile("csrr %0, pmpaddr2" : "=r" (x));
    return x;
}
static inline uint32_t r_pmpaddr3()
{
    uint32_t x;
    asm volatile("csrr %0, pmpaddr3" : "=r" (x));
    return x;
}
static inline uint32_t r_pmpaddr4()
{
    uint32_t x;
    asm volatile("csrr %0, pmpaddr4" : "=r" (x));
    return x;
}
static inline uint32_t r_pmpaddr5()
{
    uint32_t x;
    asm volatile("csrr %0, pmpaddr5" : "=r" (x));
    return x;
}
static inline uint32_t r_pmpaddr6()
{
    uint32_t x;
    asm volatile("csrr %0, pmpaddr6" : "=r" (x));
    return x;
}
static inline uint32_t r_pmpaddr7()
{
    uint32_t x;
    asm volatile("csrr %0, pmpaddr7" : "=r" (x));
    return x;
}
static inline uint32_t r_pmpaddr8()
{
    uint32_t x;
    asm volatile("csrr %0, pmpaddr8" : "=r" (x));
    return x;
}
static inline uint32_t r_pmpaddr9()
{
    uint32_t x;
    asm volatile("csrr %0, pmpaddr9" : "=r" (x));
    return x;
}
static inline uint32_t r_pmpaddr10()
{
    uint32_t x;
    asm volatile("csrr %0, pmpaddr10" : "=r" (x));
    return x;
}
static inline uint32_t r_pmpaddr11()
{
    uint32_t x;
    asm volatile("csrr %0, pmpaddr11" : "=r" (x));
    return x;
}
static inline uint32_t r_pmpaddr12()
{
    uint32_t x;
    asm volatile("csrr %0, pmpaddr12" : "=r" (x));
    return x;
}
static inline uint32_t r_pmpaddr13()
{
    uint32_t x;
    asm volatile("csrr %0, pmpaddr13" : "=r" (x));
    return x;
}
static inline uint32_t r_pmpaddr14()
{
    uint32_t x;
    asm volatile("csrr %0, pmpaddr14" : "=r" (x));
    return x;
}
static inline uint32_t r_pmpaddr15()
{
    uint32_t x;
    asm volatile("csrr %0, pmpaddr15" : "=r" (x));
    return x;
}
