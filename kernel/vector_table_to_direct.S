    /* Taken from https://github.com/espressif/esp32c3-direct-boot-example/blob/main/common/vectors.S */
    .balign 0x100
    .global _vector_table
_vector_table:
    .rept 32
    j kernelvec
    .endr
