#include <link.h>
#include <thread.h>
#include <syscall.h>

#define TIMEOUT 4

void __USER_TEXT root_thread()
{
    __asm__ __volatile__("mv a0, %0" : : "r" (SYS_IPC) : "a0");

    __asm__ __volatile__("mv a1, %0" : : "r" (L4_NILTHREAD) : "a1");

    __asm__ __volatile__("mv a2, %0" : : "r" (L4_NILTHREAD) : "a2");

    // does not work, because ecall gets corrupted/misaligned/something:
    __asm__ __volatile__("add a3, x0, %0" : : "r" (TIMEOUT) : "a3");
    // works:
    // __asm__ __volatile__("addi a3, x0, 1000"); // set timeout to 1000
    __asm__ __volatile__ (".align 2\n\t ecall");


    /* __asm__ __volatile__( */
    /*     "mv a0, %0\n\t\ */
    /*     mv a1, %1\n\t\ */
    /*     mv a2, %2\n\t\ */
    /*     addi a3, x0, 1000\n\t\ */
    /*     ecall\n\t" */
    /*     : */
    /*     : "r" (SYS_IPC), */
    /*         "r" (L4_NILTHREAD), */
    /*         "r" (L4_NILTHREAD) */
    /*     : "a0", "a1", "a2", "a3"); */

    while(1);
}
