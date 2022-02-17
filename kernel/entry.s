.section __l4_start_section
.global idle_stack_end
.global _entry
_entry:
    # Load __global_pointer$ (from linker script) into gp.
    # Should allow for linker relaxation
    .option push
    .option norelax
    1:auipc gp, %pcrel_hi(__global_pointer$)
    addi  gp, gp, %pcrel_lo(1b)
    .option pop

    # Set up a stack for C.
    la sp, kernel_stack_end
    li a0, 1024*4
    call __l4_start
spin:
    j spin
