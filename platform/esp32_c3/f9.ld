/*
 * memory area definitions
 */

INCLUDE "platform/esp32_c3/f9_mem.ld"

ENTRY (_entry)

/*
 * Memory Layout
 *
 * +-------------------+    0x0000 0000
 * +  isr_vector       +
 * +--------------------    0x0000 1000 (PROC_TABLE_BASE)
 * +  proc_table       |
 * +--------------------    0x0000 2000 (KERNEL_BASE)
 * |  text / data      | 30k
 * ~~~~~~~~~~~~~~~~~~~~~
 *
 * +-------------------+        0x1000 0000
 * + Bitmap /          +
 * + kernel AHB        +
 * +-------------------+
 *
 * +-------------------+    0x2000 0000
 * +  stack            + 1k
 * +-------------------+    0x2000 0400 (+KERNEL_STACK)
 * +  data / bss       +
 * +-------------------+
 * +..userspace........+
 * +-------------------+
 *
 * +-------------------+    0x4000 0000
 * +  APB mem          +
 * +-------------------+        0x4002 0000
 * +  AHB mem          +
 * +-------------------+
 */

SYSTEM_CALL_BASE    = 0x00000800;
KERNEL_BASE         = 0x00001000;
USERSPACE_BASE      = 0x0000C000;

IDLE_STACK_SIZE         = 0x00000200;
/* KERNEL_STACK_SIZE    = 0x00000300; */
KERNEL_STACK_SIZE   = 0x00001000;
/* ROOT_THREAD_STACK_SIZE   = 0x00000200; */
ROOT_THREAD_STACK_SIZE  = 0x00001000;
/* HOPPUS_THREAD_STACK_SIZE  = 0x00001000; */
HOPPUS_THREAD_STACK_SIZE  = 0x00008000;
/* HOPPUS_THREAD_HEAP_SIZE   = 0x00004000; */
HOPPUS_THREAD_HEAP_SIZE   = 0x00008000;

PING_THREAD_STACK_SIZE  = 0x00001000;
PONG_THREAD_STACK_SIZE  = 0x00001000;

REPLIER_THREAD_STACK_SIZE  = 0x00001000;

MEM0_SIZE = 0x00001000;

/* We need to align to 256 in order for the sections to match an fpage (because the
last 6 bits of the fpage base is used for poolid */

SECTIONS {
    .text :
    {
        KEEP(*(__l4_start_section))
        /* . = KERNEL_BASE; */
        kernel_text_start = .;
        *(.text*)
        *(.rodata*)
        kernel_text_end = .;
    } > iram

    .user_text :
    ALIGN (256)
    {
        user_text_start = .;
        *(.user_text*)
        . = ALIGN (256);
        user_text_end = .;
    } > iram
    end_of_text = .;

    .sym_tab :
    {
        symtab_start = .;
        KEEP(*(.sym_tab*))
        symtab_end = .;
    } > dram

    .stack :
    ALIGN (256)
    {
        idle_stack_start = .;
        . += IDLE_STACK_SIZE;
        . = ALIGN (256);
        idle_stack_end = .;
        . += KERNEL_STACK_SIZE;
        kernel_stack_end = .;
    } > dram

    .data :
    ALIGN (256)
    {
        kernel_data_start = .;
        kip_start = .;
        *(.kip*)
        kip_end = .;
        /* Only kernel data */
        *(.data*)
        kernel_data_end = .;
    } > dram

    /* Taken from https://www.sifive.com/blog/all-aboard-part-3-linker-relaxation-in-riscv-toolchain */
    /* We want the small data sections together, so single-instruction offsets
    can access them all, and initialized data all before uninitialized, so
    we can shorten the on-disk segment size.  */
    .sdata :
    {
        __global_pointer$ = . + 0x800;
        *(.srodata.cst16) *(.srodata.cst8) *(.srodata.cst4) *(.srodata.cst2) *(.srodata .srodata.*)
        *(.sdata .sdata.* .gnu.linkonce.s.*)
    } > dram
    . = .;
    /* zero initialized data */
    bss_start = .;
    .sbss :
    {
        *(.dynsbss)
        *(.sbss .sbss.* .gnu.linkonce.sb.*)
        *(.scommon)
    } > dram
    .bss (NOLOAD) :
    ALIGN (256)
    {
        kernel_bss_start = .;
        *(.bss*)
        *(.ktable*)         /* Statically allocated kernel table */
        *(COMMON)
        kernel_bss_end = .;
        bitmap_start = .;
        *(.bitmap*)
        bitmap_end = .;
    } > dram
    bss_end = .;


    .user_data :
    ALIGN (256)
    {
        user_data_start = .;
        root_stack_start = .;
        . += ROOT_THREAD_STACK_SIZE;
        . = ALIGN (256);
        root_stack_end = .;

        hoppus_thread_stack_start = .;
        . += HOPPUS_THREAD_STACK_SIZE;
        . = ALIGN (256);
        hoppus_thread_stack_end = .;

        hoppus_thread_heap_start = .;
        . += HOPPUS_THREAD_HEAP_SIZE;
        . = ALIGN (256);
        hoppus_thread_heap_end = .;

        user_threads_data_start = .;
        *(.user_data*)
        . = ALIGN (256);
        user_threads_data_end = .;

        ping_thread_stack_start = .;
        . += PING_THREAD_STACK_SIZE;
        . = ALIGN (256);
        ping_thread_stack_end = .;

        pong_thread_stack_start = .;
        . += PONG_THREAD_STACK_SIZE;
        . = ALIGN (256);
        pong_thread_stack_end = .;

        replier_thread_stack_start = .;
        . += REPLIER_THREAD_STACK_SIZE;
        . = ALIGN (256);
        replier_thread_stack_end = .;

        user_data_end = .;
    } > dram

    .user_bss (NOLOAD) :
    ALIGN (256)
    {
        user_bss_start = .;
        *(.user_bss*)
        . = ALIGN (256);
        user_bss_end = .;
        mem0_start = .;
        . += MEM0_SIZE;
        mem0_end = .;
    } > dram

}


PROVIDE(uart_tx_one_char = 0x40000068);
PROVIDE(uart_tx_one_char2 = 0x4000006c);
PROVIDE(uart_rx_one_char = 0x40000070);
PROVIDE(uart_rx_one_char_block = 0x40000074);
PROVIDE(uart_rx_readbuff = 0x40000078);
