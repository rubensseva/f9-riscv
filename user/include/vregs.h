#ifndef VREGS_H_
#define VREGS_H_


/* Copyright (c) 2001-2003, 2005 Karlsruhe University. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* ARM virtual registers */

#include <l4/utcb.h>
#include <platform/link.h>
#include <user_types.h>

#define __L4_NUM_MRS    16
#define __L4_NUM_BRS    8

__USER_DATA register L4_Word_t __L4_MR0 asm ("s4");
__USER_DATA register L4_Word_t __L4_MR1 asm ("s5");
__USER_DATA register L4_Word_t __L4_MR2 asm ("s6");
__USER_DATA register L4_Word_t __L4_MR3 asm ("s7");
__USER_DATA register L4_Word_t __L4_MR4 asm ("s8");
__USER_DATA register L4_Word_t __L4_MR5 asm ("s9");
__USER_DATA register L4_Word_t __L4_MR6 asm ("s10");
__USER_DATA register L4_Word_t __L4_MR7 asm ("s11");

/*
 * All virtual registers on ARM is located in a user Level Thread
 * Control Block (UTCB).
 */
__USER_TEXT L4_INLINE utcb_t *__L4_Utcb(void)
{
    extern void *current_utcb;
    return current_utcb;
}

/*
 * Location of TCRs within UTCB.
 */
#define __L4_TCR_THREAD_WORD_0          (-4)
#define __L4_TCR_THREAD_WORD_1          (-5)
#define __L4_TCR_VIRTUAL_ACTUAL_SENDER      (-6)
#define __L4_TCR_INTENDED_RECEIVER      (-7)
#define __L4_TCR_XFER_TIMEOUT           (-8)
#define __L4_TCR_ERROR_CODE             (-9)
#define __L4_TCR_PREEMPT_FLAGS          (-10)
#define __L4_TCR_COP_FLAGS          (-10)
#define __L4_TCR_EXCEPTION_HANDLER      (-11)
#define __L4_TCR_PAGER              (-12)
#define __L4_TCR_USER_DEFINED_HANDLE        (-13)
#define __L4_TCR_PROCESSOR_NO           (-14)
#define __L4_TCR_MY_GLOBAL_ID           (-15)

/*
 * Thread Control Registers.
 */
__USER_TEXT L4_INLINE L4_Word_t __L4_TCR_MyGlobalId(void)
{
    return __L4_Utcb()->t_globalid;
}

__USER_TEXT L4_INLINE L4_Word_t __L4_TCR_MyLocalId(void)
{
    return (L4_Word_t) __L4_Utcb();
}

__USER_TEXT L4_INLINE L4_Word_t __L4_TCR_ProcessorNo(void)
{
    return (__L4_Utcb())->processor_no;
}

__USER_TEXT L4_INLINE L4_Word_t __L4_TCR_UserDefinedHandle(void)
{
    return __L4_Utcb()->user_defined_handle;
}

__USER_TEXT L4_INLINE void __L4_TCR_Set_UserDefinedHandle(L4_Word_t w)
{
    __L4_Utcb()->user_defined_handle = w;
}

__USER_TEXT L4_INLINE L4_Word_t __L4_TCR_Pager(void)
{
    return __L4_Utcb()->t_pager;
}

__USER_TEXT L4_INLINE void __L4_TCR_Set_Pager(L4_Word_t w)
{
    __L4_Utcb()->t_pager = w;
}

__USER_TEXT L4_INLINE L4_Word_t __L4_TCR_ExceptionHandler(void)
{
    return __L4_Utcb()->exception_handler;
}

__USER_TEXT L4_INLINE void __L4_TCR_Set_ExceptionHandler(L4_Word_t w)
{
    __L4_Utcb()->exception_handler = w;
}

__USER_TEXT L4_INLINE L4_Word_t __L4_TCR_ErrorCode(void)
{
    return __L4_Utcb()->error_code;
}

__USER_TEXT L4_INLINE L4_Word_t __L4_TCR_XferTimeout(void)
{
    return __L4_Utcb()->xfer_timeouts;
}

__USER_TEXT L4_INLINE void __L4_TCR_Set_XferTimeout(L4_Word_t w)
{
    __L4_Utcb()->xfer_timeouts = w;
}

__USER_TEXT L4_INLINE L4_Word_t __L4_TCR_IntendedReceiver(void)
{
    return __L4_Utcb()->intended_receiver;
}

__USER_TEXT L4_INLINE L4_Word_t __L4_TCR_ActualSender(void)
{
    return __L4_Utcb()->sender;
}

__USER_TEXT L4_INLINE void __L4_TCR_Set_VirtualSender(L4_Word_t w)
{
    __L4_Utcb()->sender = w;
}

/*
 * Message Registers.
 */
__USER_TEXT L4_INLINE L4_Word_t L4_NumMRs(void)
{
    return __L4_NUM_MRS;
}


__USER_TEXT L4_INLINE void L4_StoreMR(int i, L4_Word_t *w)
{
    switch (i) {
    case 0: *w = __L4_MR0; break;
    case 1: *w = __L4_MR1; break;
    case 2: *w = __L4_MR2; break;
    case 3: *w = __L4_MR3; break;
    case 4: *w = __L4_MR4; break;
    case 5: *w = __L4_MR5; break;
    case 6: *w = __L4_MR6; break;
    case 7: *w = __L4_MR7; break;
    default:
        if (i >= 0 && i < __L4_NUM_MRS)
            *w = __L4_Utcb()->mr[i - 8];
        else
            *w = 0;
    }
}

__USER_TEXT L4_INLINE void L4_LoadMR(int i, L4_Word_t w)
{
    switch (i) {
    case 0: __L4_MR0 = w; break;
    case 1: __L4_MR1 = w; break;
    case 2: __L4_MR2 = w; break;
    case 3: __L4_MR3 = w; break;
    case 4: __L4_MR4 = w; break;
    case 5: __L4_MR5 = w; break;
    case 6: __L4_MR6 = w; break;
    case 7: __L4_MR7 = w; break;
    default:
        if (i >= 0 && i < __L4_NUM_MRS)
            __L4_Utcb()->mr[i - 8] = w;
    }
}

__USER_TEXT L4_INLINE void L4_StoreMRs(int i, int k, L4_Word_t * w)
{
    if (i < 0 || k <= 0 || i + k > __L4_NUM_MRS)
        return;

    switch (i) {
    case 0: *w++ = __L4_MR0; if (--k <= 0) break;
    case 1: *w++ = __L4_MR1; if (--k <= 0) break;
    case 2: *w++ = __L4_MR2; if (--k <= 0) break;
    case 3: *w++ = __L4_MR3; if (--k <= 0) break;
    case 4: *w++ = __L4_MR4; if (--k <= 0) break;
    case 5: *w++ = __L4_MR5; if (--k <= 0) break;
    case 6: *w++ = __L4_MR6; if (--k <= 0) break;
    case 7: *w++ = __L4_MR7; if (--k <= 0) break;
    default:
        {
            uint32_t *mr = __L4_Utcb()->mr;
            while (k-- > 0)
                *w++ = *mr++;
        }
    }
}

__USER_TEXT L4_INLINE void L4_LoadMRs(int i, int k, L4_Word_t *w)
{
    if (i < 0 || k <= 0 || i + k > __L4_NUM_MRS)
        return;

    switch (i) {
    case 0: __L4_MR0 = *w++; if (--k <= 0) break;
    case 1: __L4_MR1 = *w++; if (--k <= 0) break;
    case 2: __L4_MR2 = *w++; if (--k <= 0) break;
    case 3: __L4_MR3 = *w++; if (--k <= 0) break;
    case 4: __L4_MR4 = *w++; if (--k <= 0) break;
    case 5: __L4_MR5 = *w++; if (--k <= 0) break;
    case 6: __L4_MR6 = *w++; if (--k <= 0) break;
    case 7: __L4_MR7 = *w++; if (--k <= 0) break;
    default:
        {
            uint32_t *mr = __L4_Utcb()->mr;
            while (k-- > 0)
                *mr++ = *w++;
        }
    }
}

#endif // VREGS_H_
