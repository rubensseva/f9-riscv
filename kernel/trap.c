#include <stdint.h>
#include <syscall.h>
#include <ktimer.h>
#include <syscall.h>
#include <riscv.h>
#include <irq.h>
#include <interrupt.h>
#include <debug.h>
#include <thread.h>
#include <memory.h>
#include <mpu.h>
#include <utility.h>
#include <config.h>
#include <ESP32_C3.h>
#include <error.h>
#include <uart_ESP32_C3.h>

extern void timervec();
extern void kernel_vec_in_c_restore();

/* TODO: Should this function really be here? Would be nice to have available in other files as well */
void dump_state() {
    dump_threads();
    dump_mpu();
    dump_current_thread();
}

void access_fault_handler(void) {
    /* mtval will contain the correct faulting address. For example, on an
       instruction access fault, it will contain the instruction. On a load access
       fault, it will contain the address that caused that load to fail. */
    uint32_t fault_addr = r_mtval();
    /* Try to setup an mpu region for the address that cause the exception.
       If the address is not in the threads address space, nothing will happen. */
    if (mpu_select_lru(current->as, fault_addr) != 0) {
        /* TODO: What should we do here? Probably send the exception as an IPC message to pager? */
        uint32_t mcause = r_mcause();
        uint32_t mstatus = r_mstatus();
        uint32_t mepc = r_mepc();
        dbg_printf(DL_EMERG,
                   "\n------ACCESS FAULT------\nmcause: 0x%x, mstaus: 0x%x, mepc: 0x%x, mtval: 0x%x, thread_id: %d\n",
                   mcause, mstatus, mepc, fault_addr, current->t_globalid);
        dump_state();
        panic("Unrecoverable access fault");
    }
}

void unimplemented(void) {
    uint32_t mtval = r_mtval();
    uint32_t mcause = r_mcause();
    uint32_t mstatus = r_mstatus();
    uint32_t mepc = r_mepc();
    dbg_printf(DL_EMERG,
               "\n------UNIMPLEMENTED TRAP------\nmcause: 0x%x, mstaus: 0x%x, mepc: 0x%x, mtval: 0x%x, thread_id: %d\n",
               mcause, mstatus, mepc, mtval, current->t_globalid);
    dump_state();
    panic("Unrecoverable error, unimplemented trap");
}

/* interrupt handlers start */

void machine_timer_interrupt_handler(void) {

    /* uint32_t *clint_mtimecmp = (uint32_t*)CLINT_MTIMECMP; */
    /* uint32_t *clint_mtime = (uint32_t*)CLINT_MTIME; */
    /* uint32_t interval = 1000000; // cycles; about 1/10th second in qemu. */
    /* uint32_t new_val = *clint_mtime + interval; */
    /* *clint_mtimecmp = new_val; */

    ktimer_handler();
}

void supervisor_timer_interrupt_handler(void) {
    panic("ERROR: Got supervisor timer interrupt, shouldnt happen\n");
}

void supervisor_external_interrupt(void) {
    panic("ERROR: Got supervisor external interrupt, shouldnt happen\n");
}

/* interrupt handlers end */

/* exception handlers start */
/* access fault handlers start */
void illegal_instruction_access_fault_handler(void) {
    access_fault_handler();
}

void load_access_fault_handler(void) {
    access_fault_handler();
}

void store_or_AMO_access_fault_handler(void) {
    access_fault_handler();
}
/* access fault handlers end */

void instruction_address_misaligned_handler(void) {
    dbg_printf(DL_EMERG, "Instruction address misaligned exception. mepc: %x, mtval: %x\n",
               r_mepc(), r_mtval());
    dump_state();
}
void illegal_instruction_handler(void) {
    dbg_printf(DL_EMERG, "Illegal instruction exception. mepc: %x, mtval: %x\n",
               r_mepc(), r_mtval());
    dump_state();
}
void load_address_misaligned_handler(void) {
    dbg_printf(DL_EMERG, "Load address misalgined exception. mepc: %x, mtval: %x\n",
               r_mepc(), r_mtval());
    dump_state();
}
void store_or_AMO_address_misaligned_handler(void) {
    dbg_printf(DL_EMERG, "Store/AMO address misaligned exception. mepc: %x, mtval: %x\n",
               r_mepc(), r_mtval());
    dump_state();
    panic("Unrecoverable Store/AMO address misaligned exception");
}

void ecall_from_u_handler(void) {
    current->ctx.mepc = r_mepc() + 4;
    svc_handler();
}
void ecall_from_s_handler(void) {
    dbg_printf(DL_EMERG, "Ecall from supervisor mode, this should never happen. Trying to proceed. mepc: %x, mstatus: %x\n",
               r_mepc(), r_mstatus());
    dump_state();
    current->ctx.mepc = r_mepc() + 4;
    svc_handler();
}
void ecall_from_m_handler(void) {
    current->ctx.mepc = r_mepc() + 4;
    svc_handler();
}

/* exception handlers end */


void (*interrupt_handlers[12])() = {
    unimplemented,
    unimplemented,
    unimplemented,
    unimplemented,
    unimplemented,
    supervisor_timer_interrupt_handler,
    unimplemented,
    machine_timer_interrupt_handler,
    unimplemented,
    supervisor_external_interrupt, // Interrupts to PLIC goes here
    unimplemented,
    unimplemented,
};
void (*exception_handlers[16])() = {
    instruction_address_misaligned_handler,
    illegal_instruction_access_fault_handler,
    illegal_instruction_handler,
    unimplemented, // breakpoint
    load_address_misaligned_handler,
    load_access_fault_handler,
    store_or_AMO_address_misaligned_handler,
    store_or_AMO_access_fault_handler,
    ecall_from_u_handler,
    ecall_from_s_handler,
    unimplemented,
    ecall_from_m_handler, // should only happen from kernel thread
    unimplemented,
    unimplemented,
    unimplemented,
    unimplemented,
};


#define MCAUSE_INT_MASK 0x80000000 // [31]=1 interrupt, else exception
#define MCAUSE_CODE_MASK 0x7FFFFFFF // low bits show code

extern void kerneltrap(uint32_t* caller_sp)
{
    unsigned long mcause_value = r_mcause();

    current->ctx.mepc = r_mepc();
    if (mcause_value & MCAUSE_INT_MASK) {
        if ((mcause_value & MCAUSE_CODE_MASK) == CONFIG_SYSTEM_TIMER_CPU_INTR) {
            machine_timer_interrupt_handler();

            /* Clear system timer interrupt */
            volatile uint32_t *systimer_target0_int_clr = REG(SYSTEM_TIMER_BASE + SYSTIMER_INT_CLR_REG);
            *systimer_target0_int_clr = 1; // clear TARGET0
        } else {

            /* TODO: Remove this when no longer needed */
            /* if ((mcause_value & MCAUSE_CODE_MASK) == CONFIG_UART_CPU_INTR) { */
            /*     while(UART_rxfifo_count(0) != 0) { */
            /*         dbg_printf(DL_BASIC, "%c", (UART_read(0))); */
            /*     } */
            /* } */
            /* Try to clear using interrupt matrix (only works for edge type interrupts) */
            volatile uint32_t *interrupt_core0_cpu_int_clear = REG(INTERRUPT_MATRIX_BASE + INTERRUPT_CORE0_CPU_INT_CLEAR_REG);
            *interrupt_core0_cpu_int_clear |= (1 << CONFIG_UART_CPU_INTR);
            /* Try to clear from source */
            /* TODO: I dont think we can just clear the CPU interrupt, we need to clear the specific UART interrupt that happened */
            /* volatile uint32_t *uart_int_clr = REG(UART_CONTROLLER_0_BASE + UART_INT_CLR_REG); */
            /* *uart_int_clr |= (1 << UART_INTR__UART_RXFIFO_FULL); */

            __interrupt_handler(mcause_value & MCAUSE_CODE_MASK);
        }
    } else {
        exception_handlers[(mcause_value & MCAUSE_CODE_MASK)]();
    }

    /* Context switch */
    current->ctx.sp = (uint32_t) caller_sp;
    tcb_t* sel = schedule_select();
    if (sel != current) {
        thread_switch(sel);
    }

    /* Kernel thread should run in m-mode, rest should run in u-mode
     * The reason we need the kernel thread in m-mode is because it needs
     * the ability to disable all interrupts */
    /* TODO: Consider running idle thread in user mode */
    /* TODO: Decide on using either *current or thread_current() */
    extern tcb_t *kernel, *idle;
    unsigned long x = r_mstatus();
    x &= ~MSTATUS_MPP_MASK;
    if (thread_current() == kernel || thread_current() == idle) {
        x |= MSTATUS_MPP_M;
    } else {
        x |= MSTATUS_MPP_U;
    }
    w_mstatus(x);

    __asm__ ("csrw mepc, %0" : : "r" (current->ctx.mepc));
    __asm__ volatile ("mv a0, %0" : : "r" (current->ctx.sp));
}
