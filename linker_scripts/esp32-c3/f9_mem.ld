/*
 * memory definitions
 */

MEMORY
{
    /* OLD: */
    /* WARNING: I think this is lacking one zero to work with -kernel in qemu */
    /* MFlash (rx)  : ORIGIN = 0x08000000, LENGTH = 1024K */
    /* RamLoc (rwx) : ORIGIN = 0x20000000, LENGTH =  112K */
    /* RamCCM (rwx) : ORIGIN = 0x10000000, LENGTH =   64K */
    /* RamLoc (rwx) : ORIGIN = 0x080000000, LENGTH =  112K */
    InternalData (rwx) : ORIGIN = 0x3FC80000, LENGTH = 384K
    InternalInstructions (rwx) : ORIGIN = 0x40370000, LENGTH = 400K
    /* RamLoc (rwx) : ORIGIN = 0x080000000, LENGTH =  224K */
    /* RamCCM (rwx) : ORIGIN = 0x10000000, LENGTH =   64K */
}
