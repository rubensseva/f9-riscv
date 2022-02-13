/*
 * memory definitions
 */

MEMORY
{
    dram (rw)  : ORIGIN = 0x3FC80000, LENGTH = 384K
    iram (rwx) : ORIGIN = 0x40380000, LENGTH = 384K
}
