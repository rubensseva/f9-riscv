    /* Taken from https://github.com/espressif/esp32c3-direct-boot-example/blob/main/common/vectors.S */

    /* This is the vector table. MTVEC points here.
     *
     * Use 4-byte intructions here. 1 instruction = 1 entry of the table.
     * The CPU jumps to MTVEC (i.e. the first entry) in case of an exception,
     * and (MTVEC & 0xfffffffc) + (mcause & 0x7fffffff) * 4, in case of an interrupt.
     */
    .balign 0x100
    .global _vector_table
_vector_table:
    j kernelvec
    .rept 31
    j kernelvec
    .endr
