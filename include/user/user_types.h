#ifndef USER_TYPES_H_
#define USER_TYPES_H_

#include <stdint.h>

#define L4_INLINE       static inline __attribute__((always_inline))

typedef void (*irq_handler_t)(void);
// typedef uint32_t L4_Word_t;
// typedef L4_Word_t L4_ThreadId_t;

// #define L4_nilthread  0UL
/* TODO: Use this instead #define L4_nilthread ((L4_ThreadId_t) { raw : 0UL}) */


/* FIXME: specify ARM Cortex-M in filename */
#define L4_32BIT
#define L4_LITTLE_ENDIAN

typedef unsigned int __attribute__((__mode__(__DI__))) L4_Word64_t;
typedef unsigned int        L4_Word32_t;
typedef unsigned short      L4_Word16_t;
typedef unsigned char       L4_Word8_t;

typedef unsigned long       L4_Word_t;

typedef signed int __attribute__((__mode__(__DI__))) L4_SignedWord64_t;
typedef signed int      L4_SignedWord32_t;
typedef signed short        L4_SignedWord16_t;
typedef signed char         L4_SignedWord8_t;

typedef signed long         L4_SignedWord_t;

typedef unsigned int        L4_Size_t;
typedef L4_Word64_t         L4_Paddr_t;



#undef L4_32BIT
#undef L4_64BIT
#undef L4_BIG_ENDIAN
#undef L4_LITTLE_ENDIAN

/*
 * All types used within <arch/syscalls.h> should be declared in this
 * file.
 */
#define L4_HAVE_SIGNED_WORD

typedef L4_Word_t       L4_Bool_t;

#define L4_INLINE       static inline __attribute__((always_inline))
#define L4_BITS_PER_WORD    (sizeof (L4_Word_t) * 8)

#if defined(L4_64BIT)
# define __PLUS32   + (sizeof (L4_Word_t) * 8 - 32)
# define __14       32
# define __18       32
#else
# define __PLUS32   + 0
# define __14       14
# define __18       18
#endif

#if defined(L4_BIG_ENDIAN)
#define L4_BITFIELD2(t,a,b)         t b; t a
#define L4_BITFIELD3(t,a,b,c)       t c; t b; t a
#define L4_BITFIELD4(t,a,b,c,d)         t d; t c; t b; t a
#define L4_BITFIELD5(t,a,b,c,d,e)   t e; t d; t c; t b; t a
#define L4_BITFIELD6(t,a,b,c,d,e,f)     t f; t e; t d; t c; t b; t a
#define L4_BITFIELD7(t,a,b,c,d,e,f,g)   t g; t f; t e; t d; t c; t b; t a
#define L4_BITFIELD8(t,a,b,c,d,e,f,g,h)     t h; t g; t f; t e; t d; t c; t b; t a
#define L4_BITFIELD9(t,a,b,c,d,e,f,g,h,i)   \
    t i; t h; t g; t f; t e; t d; t c; t b; t a
#define L4_SHUFFLE2(a,b)        b,a
#define L4_SHUFFLE3(a,b,c)      c,b,a
#define L4_SHUFFLE4(a,b,c,d)        d,c,b,a
#define L4_SHUFFLE5(a,b,c,d,e)      e,d,c,b,a
#define L4_SHUFFLE6(a,b,c,d,e,f)    f,e,d,c,b,a
#define L4_SHUFFLE7(a,b,c,d,e,f,g)  g,f,e,d,c,b,a
#else
#define L4_BITFIELD2(t,a,b)         t a; t b
#define L4_BITFIELD3(t,a,b,c)       t a; t b; t c
#define L4_BITFIELD4(t,a,b,c,d)         t a; t b; t c; t d
#define L4_BITFIELD5(t,a,b,c,d,e)   t a; t b; t c; t d; t e
#define L4_BITFIELD6(t,a,b,c,d,e,f)     t a; t b; t c; t d; t e; t f
#define L4_BITFIELD7(t,a,b,c,d,e,f,g)   t a; t b; t c; t d; t e; t f; t g
#define L4_BITFIELD8(t,a,b,c,d,e,f,g,h)     t a; t b; t c; t d; t e; t f; t g; t h
#define L4_BITFIELD9(t,a,b,c,d,e,f,g,h,i)   \
    t a; t b; t c; t d; t e; t f; t g; t h; t i
#define L4_SHUFFLE2(a,b)        a,b
#define L4_SHUFFLE3(a,b,c)      a,b,c
#define L4_SHUFFLE4(a,b,c,d)        a,b,c,d
#define L4_SHUFFLE5(a,b,c,d,e)      a,b,c,d,e
#define L4_SHUFFLE6(a,b,c,d,e,f)    a,b,c,d,e,f
#define L4_SHUFFLE7(a,b,c,d,e,f,g)  a,b,c,d,e,f,g
#endif

/*
 * Error codes
 */

#define L4_ErrOk        (0)
#define L4_ErrNoPrivilege   (1)
#define L4_ErrInvalidThread     (2)
#define L4_ErrInvalidSpace  (3)
#define L4_ErrInvalidScheduler  (4)
#define L4_ErrInvalidParam  (5)
#define L4_ErrUtcbArea      (6)
#define L4_ErrKipArea       (7)
#define L4_ErrNoMem         (8)

/*
 * Fpages
 */
typedef union {
    L4_Word_t raw;
    struct {
        L4_BITFIELD4(L4_Word_t,
                rwx : 3,
                extended : 1,
                s : 6,
                b : 22 __PLUS32);
    } X;
} L4_Fpage_t;

#define L4_Readable         (0x04)
#define L4_Writable         (0x02)
#define L4_eXecutable       (0x01)
#define L4_FullyAccessible  (0x07)
#define L4_ReadWriteOnly    (0x06)
#define L4_ReadeXecOnly         (0x05)
#define L4_NoAccess         (0x00)

#define L4_Nilpage      ((L4_Fpage_t) { raw : 0UL })
#define L4_CompleteAddressSpace     ((L4_Fpage_t) { X : { L4_SHUFFLE4(0, 0, 1, 0) }})



L4_INLINE L4_Word_t L4_Address(L4_Fpage_t f)
{
    return f.raw & ~((1UL << f.X.s) - 1);
}

L4_INLINE L4_Word_t L4_Size(L4_Fpage_t f)
{
    return f.X.s == 0 ? 0 : (1UL << f.X.s);
}

L4_INLINE L4_Word_t L4_SizeLog2(L4_Fpage_t f)
{
    return f.X.s;
}

/*
 * Thread IDs
 */
typedef union {
    L4_Word_t raw;
    struct {
        L4_BITFIELD2(L4_Word_t,
                version : __14,
                thread_no : __18);
    } X;
} L4_GthreadId_t;

typedef union {
    L4_Word_t raw;
    struct {
        L4_BITFIELD2(L4_Word_t,
                __zeros : 6,
                local_id : 26 __PLUS32);
    } X;
} L4_LthreadId_t;

typedef union {
    L4_Word_t raw;
    L4_GthreadId_t global;
    L4_LthreadId_t local;
} L4_ThreadId_t;

#define L4_nilthread        ((L4_ThreadId_t) { raw : 0UL})
#define L4_anythread        ((L4_ThreadId_t) { raw : ~0UL})
#define L4_anylocalthread   ((L4_ThreadId_t) { local : { X : \
                    { L4_SHUFFLE2(0, \
                        ((1UL << (8 * sizeof(L4_Word_t) - 6)) - 1)) \
                    } } })

L4_INLINE L4_ThreadId_t L4_GlobalId(L4_Word_t threadno, L4_Word_t version)
{
    L4_ThreadId_t t;
    t.global.X.thread_no = threadno;
    t.global.X.version = version;
    return t;
}

L4_INLINE L4_Word_t L4_Version(L4_ThreadId_t t)
{
    return t.global.X.version;
}

L4_INLINE L4_Word_t L4_ThreadNo(L4_ThreadId_t t)
{
    return t.global.X.thread_no;
}

L4_INLINE L4_Bool_t L4_IsThreadEqual(
        const L4_ThreadId_t l,
        const L4_ThreadId_t r)
{
    return l.raw == r.raw;
}

L4_INLINE L4_Bool_t L4_IsThreadNotEqual(
        const L4_ThreadId_t l,
        const L4_ThreadId_t r)
{
    return l.raw != r.raw;
}

L4_INLINE L4_Bool_t L4_IsNilThread(L4_ThreadId_t t)
{
    return t.raw == 0;
}

L4_INLINE L4_Bool_t L4_IsLocalId(L4_ThreadId_t t)
{
    return t.local.X.__zeros == 0;
}

L4_INLINE L4_Bool_t L4_IsGlobalId(L4_ThreadId_t t)
{
    return t.local.X.__zeros != 0;
}

/*
 * Time
 */
typedef union {
    L4_Word16_t raw;
    struct {
        L4_BITFIELD3(L4_Word_t,
                m : 10,
                e : 5,
                a : 1);
    } period;
    struct {
        L4_BITFIELD4(L4_Word_t,
                m : 10,
                c : 1,
                e : 4,
                a : 1);
    } point;
} L4_Time_t;

#define L4_Never    ((L4_Time_t) { raw : 0UL })
#define L4_ZeroTime     ((L4_Time_t) { period : { L4_SHUFFLE3(0, 1, 0) }})


#undef __14
#undef __18
#undef __PLUS32

#endif // USER_TYPES_H_
