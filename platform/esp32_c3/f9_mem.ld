/*
 * memory definitions
 */

MEMORY
{
    iram (rwx) : ORIGIN = 0x40380000, LENGTH = 200K
    dram (rw)  : ORIGIN = 0x3FC80000 + LENGTH(iram), LENGTH = 175K
}
