	# qemu -kernel loads the kernel at 0x80000000
    # and causes each CPU to jump there.
    # kernel.ld causes the following code to
    # be placed at 0x80000000.
.section __l4_start_section
.global idle_stack_end
.global _entry
_entry:
	# set up a stack for C.
    # stack0 is declared in start.c,
    # with a 4096-byte stack per CPU.
    # sp = stack0 + (hartid * 4096)
    la sp, kernel_stack_end
    li a0, 1024*4
	# jump to start() in start.c
    call __l4_start
spin:
    j spin
