#include <thread.h>
#include <trap.h>

extern volatile tcb_t *current;

__attribute__((naked)) void store_ctx() {
    __asm__ ("mv %0, ra" : "=r" (current->ctx.ra));
    __asm__ ("mv %0, sp" : "=r" (current->ctx.sp));
    __asm__ ("mv %0, gp" : "=r" (current->ctx.gp));
    __asm__ ("mv %0, tp" : "=r" (current->ctx.tp));

	__asm__ ("mv %0, t0" : "=r" (current->ctx.t_regs[0]));
	__asm__ ("mv %0, t1" : "=r" (current->ctx.t_regs[1]));
	__asm__ ("mv %0, t2" : "=r" (current->ctx.t_regs[2]));
	__asm__ ("mv %0, t3" : "=r" (current->ctx.t_regs[3]));
	__asm__ ("mv %0, t4" : "=r" (current->ctx.t_regs[4]));
	__asm__ ("mv %0, t5" : "=r" (current->ctx.t_regs[5]));
	__asm__ ("mv %0, t6" : "=r" (current->ctx.t_regs[6]));

	__asm__ ("mv %0, s0" : "=r" (current->ctx.s_regs[0]));
	__asm__ ("mv %0, s1" : "=r" (current->ctx.s_regs[1]));
	__asm__ ("mv %0, s2" : "=r" (current->ctx.s_regs[2]));
	__asm__ ("mv %0, s3" : "=r" (current->ctx.s_regs[3]));
	__asm__ ("mv %0, s4" : "=r" (current->ctx.s_regs[4]));
	__asm__ ("mv %0, s5" : "=r" (current->ctx.s_regs[5]));
	__asm__ ("mv %0, s6" : "=r" (current->ctx.s_regs[6]));
	__asm__ ("mv %0, s7" : "=r" (current->ctx.s_regs[7]));
	__asm__ ("mv %0, s8" : "=r" (current->ctx.s_regs[8]));
	__asm__ ("mv %0, s9" : "=r" (current->ctx.s_regs[9]));
	__asm__ ("mv %0, s10" : "=r" (current->ctx.s_regs[10]));
	__asm__ ("mv %0, s11" : "=r" (current->ctx.s_regs[11]));

	__asm__ ("mv %0, a0" : "=r" (current->ctx.a_regs[0]));
	__asm__ ("mv %0, a1" : "=r" (current->ctx.a_regs[1]));
	__asm__ ("mv %0, a2" : "=r" (current->ctx.a_regs[2]));
	__asm__ ("mv %0, a3" : "=r" (current->ctx.a_regs[3]));
	__asm__ ("mv %0, a4" : "=r" (current->ctx.a_regs[4]));
	__asm__ ("mv %0, a5" : "=r" (current->ctx.a_regs[5]));
	__asm__ ("mv %0, a6" : "=r" (current->ctx.a_regs[6]));
	__asm__ ("mv %0, a7" : "=r" (current->ctx.a_regs[7]));

    __asm__ __volatile__ ("call kerneltrap");
}

__attribute__((naked)) void kernel_vec_in_c_restore() {
	// Not doing this anymore, since we do it in kerneltrap depending
	// on if its an interrupt or exception
	// Add 4 to mepc, so we dont repeat the same instruction
	/* __asm__ ("mv t0, %0" : : "r" (current->ctx.mepc)); */
	/* __asm__ ("addi t0, t0, 4"); */
	/* __asm__ ("csrw mepc, t0"); */

	__asm__ ("csrw mepc, %0" : : "r" (current->ctx.mepc));

    __asm__ ("mv ra, %0" : : "r" (current->ctx.ra));
    __asm__ ("mv sp, %0" : : "r" (current->ctx.sp));
    __asm__ ("mv gp, %0" : : "r" (current->ctx.gp));
    __asm__ ("mv tp, %0" : : "r" (current->ctx.tp));

	__asm__ ("mv t0, %0" : : "r" (current->ctx.t_regs[0]));
	__asm__ ("mv t1, %0" : : "r" (current->ctx.t_regs[1]));
	__asm__ ("mv t2, %0" : : "r" (current->ctx.t_regs[2]));
	__asm__ ("mv t3, %0" : : "r" (current->ctx.t_regs[3]));
	__asm__ ("mv t4, %0" : : "r" (current->ctx.t_regs[4]));
	__asm__ ("mv t5, %0" : : "r" (current->ctx.t_regs[5]));
	__asm__ ("mv t6, %0" : : "r" (current->ctx.t_regs[6]));

	__asm__ ("mv s0, %0" : : "r" (current->ctx.s_regs[0]));
	__asm__ ("mv s1, %0" : : "r" (current->ctx.s_regs[1]));
	__asm__ ("mv s2, %0" : : "r" (current->ctx.s_regs[2]));
	__asm__ ("mv s3, %0" : : "r" (current->ctx.s_regs[3]));
	__asm__ ("mv s4, %0" : : "r" (current->ctx.s_regs[4]));
	__asm__ ("mv s5, %0" : : "r" (current->ctx.s_regs[5]));
	__asm__ ("mv s6, %0" : : "r" (current->ctx.s_regs[6]));
	__asm__ ("mv s7, %0" : : "r" (current->ctx.s_regs[7]));
	__asm__ ("mv s8, %0" : : "r" (current->ctx.s_regs[8]));
	__asm__ ("mv s9, %0" : : "r" (current->ctx.s_regs[9]));
	__asm__ ("mv s10, %0" : : "r" (current->ctx.s_regs[10]));
	__asm__ ("mv s11, %0" : : "r" (current->ctx.s_regs[11]));

	__asm__ ("mv a0, %0" : : "r" (current->ctx.a_regs[0]));
	__asm__ ("mv a1, %0" : : "r" (current->ctx.a_regs[1]));
	__asm__ ("mv a2, %0" : : "r" (current->ctx.a_regs[2]));
	__asm__ ("mv a3, %0" : : "r" (current->ctx.a_regs[3]));
	__asm__ ("mv a4, %0" : : "r" (current->ctx.a_regs[4]));
	__asm__ ("mv a5, %0" : : "r" (current->ctx.a_regs[5]));
	__asm__ ("mv a6, %0" : : "r" (current->ctx.a_regs[6]));
	__asm__ ("mv a7, %0" : : "r" (current->ctx.a_regs[7]));

    __asm__ __volatile__ ("mret");
}
