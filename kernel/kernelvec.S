        #
        # interrupts and exceptions while in machine
        # mode come here.
        #
        # push all registers, call kerneltrap(), restore, return.
        #
.globl kernelvec
.align 4
kernelvec:
        # make room to save registers.
        addi sp, sp, -128

        # save the registers.
        sw a0, 0(sp)
        sw a1, 4(sp)
        sw a2, 8(sp)
        sw a3, 12(sp)
        sw a4, 16(sp)
        sw a5, 20(sp)
        sw a6, 24(sp)
        sw a7, 28(sp)
        # skip stack pointer, we'll pass it to kerneltrap with a0
        # sd sp, 32(sp)
        sw ra, 36(sp)
        sw gp, 40(sp)
        sw tp, 44(sp)
        sw t0, 48(sp)
        sw t1, 52(sp)
        sw t2, 56(sp)
        sw t3, 60(sp)
        sw t4, 64(sp)
        sw t5, 68(sp)
        sw t6, 72(sp)
        sw s0, 76(sp)
        sw s1, 80(sp)
        sw s2, 84(sp)
        sw s3, 88(sp)
        sw s4, 92(sp)
        sw s5, 96(sp)
        sw s6, 100(sp)
        sw s7, 104(sp)
        sw s8, 108(sp)
        sw s9, 112(sp)
        sw s10, 116(sp)
        sw s11, 120(sp)

        mv a0, sp
        la sp, kernel_stack_end

        call kerneltrap

        # after kerneltrap, the new stack pointer (possible the same stack pointer
        # from the same thread, if no context switch happened) is in a0
        mv sp, a0

        # restore registers.
        lw a0, 0(sp)
        lw a1, 4(sp)
        lw a2, 8(sp)
        lw a3, 12(sp)
        lw a4, 16(sp)
        lw a5, 20(sp)
        lw a6, 24(sp)
        lw a7, 28(sp)
        # skip sp
        # ld sp, 32(sp)
        lw ra, 36(sp)
        lw gp, 40(sp)
        lw tp, 44(sp)
        lw t0, 48(sp)
        lw t1, 52(sp)
        lw t2, 56(sp)
        lw t3, 60(sp)
        lw t4, 64(sp)
        lw t5, 68(sp)
        lw t6, 72(sp)
        lw s0, 76(sp)
        lw s1, 80(sp)
        lw s2, 84(sp)
        lw s3, 88(sp)
        lw s4, 92(sp)
        lw s5, 96(sp)
        lw s6, 100(sp)
        lw s7, 104(sp)
        lw s8, 108(sp)
        lw s9, 112(sp)
        lw s10, 116(sp)
        lw s11, 120(sp)

        addi sp, sp, 128


        mret
