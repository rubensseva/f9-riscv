#include <platform/mpu.h>
#include <platform/riscv.h>
#include <thread.h>
#include <debug.h>
#include <config.h>
#include <fpage.h>


void (*w_pmpaddrarr[16])(uint32_t) = {
    w_pmpaddr0,
    w_pmpaddr1,
    w_pmpaddr2,
    w_pmpaddr3,
    w_pmpaddr4,
    w_pmpaddr5,
    w_pmpaddr6,
    w_pmpaddr7,
    w_pmpaddr8,
    w_pmpaddr9,
    w_pmpaddr10,
    w_pmpaddr11,
    w_pmpaddr12,
    w_pmpaddr13,
    w_pmpaddr14,
    w_pmpaddr15,
};
uint32_t (*r_pmpaddrarr[16])() = {
    r_pmpaddr0,
    r_pmpaddr1,
    r_pmpaddr2,
    r_pmpaddr3,
    r_pmpaddr4,
    r_pmpaddr5,
    r_pmpaddr6,
    r_pmpaddr7,
    r_pmpaddr8,
    r_pmpaddr9,
    r_pmpaddr10,
    r_pmpaddr11,
    r_pmpaddr12,
    r_pmpaddr13,
    r_pmpaddr14,
    r_pmpaddr15,
};

/* w_pmpaddri writes a pmp addr register based on the pmp entry (1-16). */
void w_pmpaddri(int pmp_entry, uint32_t data) {
    w_pmpaddrarr[pmp_entry](data);
}
/* r_pmpaddri reads a pmp addr register based on the pmp entry (1-16). */
uint32_t r_pmpaddri(int pmp_entry) {
    return r_pmpaddrarr[pmp_entry]();
}

/* w_pmpcfgi_region writes a pmp cfg register based on the pmp entry (1-16).
 * Its a bit complex since the cfg configuration is densly packed in
 * registers cfg0-cfg3 (in rv32). Four bytes in each cfg register correspond
 * to the configuration of the 16 pmp entries. */
void w_pmpcfgi_region(int pmp_entry, uint8_t new_data_region, uint32_t old_data) {
    // Get the shift for the correct byte in the cfg register
    int shift = (pmp_entry % 4) * 8;
    uint32_t mask = 0xFF << shift;
    uint32_t shifted = new_data_region << shift;
    // Clear out the old data in the region we want to operate on in the chosen cfg
    uint32_t old_masked = old_data & ~mask;
    uint32_t final = old_masked | shifted;

    if (pmp_entry < 4) {
        w_pmpcfg0(final);
    } else if (pmp_entry < 8) {
        w_pmpcfg1(final);
    } else if (pmp_entry < 12) {
        w_pmpcfg2(final);
    } else {
        w_pmpcfg3(final);
    }
}

uint32_t r_pmpcfgi(int pmp_entry) {
    if (pmp_entry < 4) {
        return r_pmpcfg0();
    } else if (pmp_entry < 8) {
        return r_pmpcfg1();
    } else if (pmp_entry < 12) {
        return r_pmpcfg2();
    } else {
        return r_pmpcfg3();
    }
}

/* mpu_setup_region sets up pmp for an fpage.
 * n is the number for a range corresponding to two pmp entries. For example:
 *     n = 0 means pmp entries 0 and 1
 *     n = 3 means pmp entries 6 and 7 */
void mpu_setup_region(int n, fpage_t *fp) {
    if (n > (CONFIG_MAX_MAPPED_THREAD_FPAGES - 1)) {
        dbg_printf(DL_MEMORY, "MEMORY: ignoring request to activate fpage num %d since the maximum number of fpages has been activated: %d\n", n, CONFIG_MAX_MAPPED_THREAD_FPAGES);
        return;
    }
    int pmp_entry_lower = n * 2;
    int pmp_entry_upper = pmp_entry_lower + 1;
    uint32_t old_cfg_data = r_pmpcfgi(pmp_entry_upper);

    if (fp) {
        w_pmpaddri(pmp_entry_lower, (FPAGE_BASE(fp) >> 2));
        w_pmpaddri(pmp_entry_upper, (FPAGE_END(fp) >> 2));

        /* Check if we need to write to cfg, perform write if necessary */
        int shift = (pmp_entry_upper % 4) * 8;
        uint32_t mask = 0xFF << shift;
        uint32_t masked = old_cfg_data & mask;
        uint32_t shifted = masked >> shift;
        if ((shifted & 0xF) != 0xF) {
            w_pmpcfgi_region(pmp_entry_upper, 0xF, old_cfg_data);
        }
    } else {
        /* Clear region */
        w_pmpcfgi_region(pmp_entry_upper, 0x0, old_cfg_data);
    }
}


int addr_in_mpu(uint32_t addr)
{
    for (int i = 0; i < 8; ++i) {
        int pmp_entry_lower = i * 2;
        int pmp_entry_upper = pmp_entry_lower + 1;
        uint32_t lower_addr = r_pmpaddri(pmp_entry_lower);
        uint32_t upper_addr = r_pmpaddri(pmp_entry_upper);

        uint32_t cfg_data = r_pmpcfgi(pmp_entry_upper);
        /* Check if we need to write to cfg, perform write if necessary */
        int shift = (pmp_entry_upper % 4) * 8;
        uint32_t mask = 0xFF << shift;
        uint32_t masked = cfg_data & mask;
        uint32_t shifted = masked >> shift;

        if ((shifted & 0xF) && (addr <= lower_addr && addr < upper_addr)) {
            return 1;
        }
    }
    return 0;
}

/* mpu_select_lru checks if address addr is in the addres space as, and then
   sets up an mpu region for it. It uses the FIFO list as->mpu_first to find
   what fpages to remove if there are not enough mpu entries */
int mpu_select_lru(as_t *as, uint32_t addr)
{
    fpage_t *fp = NULL;
    int i;

    /* Kernel fault? */
    if (!as)
        return 1;

    if (addr_in_mpu(addr))
        return 1;

    fp = as->first;
    while (fp) {
        if (addr_in_fpage(addr, fp, 0)) {
            fpage_t *sfp = as->mpu_stack_first;

            /* Remove the fpage from the list first, otherwise we might get a circular list */
            remove_fpage_from_list(as, fp, mpu_first, mpu_next);

            fp->mpu_next = as->mpu_first;
            as->mpu_first = fp;

            /* Get first avalible MPU index */
            i = 0;
            while (sfp) {
                ++i;
                sfp = sfp->mpu_next;
            }

            /* Update MPU */
            mpu_setup_region(i++, fp);

            while (i < 8 && fp->mpu_next) {
                mpu_setup_region(i++, fp->mpu_next);
                fp = fp->mpu_next;
            }

            return 0;
        }

        fp = fp->as_next;
    }
    return 1;
}


/**
   Map the interrupt vector to a pmp fpage. This is needed when an interrupt happens in
   user mode, otherwise it triggers an illegal instruction access exception.
 */
void map_intr_vector(uint32_t vector_table_ptr) {
    w_pmpaddr14(vector_table_ptr >> 2);
    w_pmpaddr15((vector_table_ptr + (4 * 32)) >> 2); // We have 32 instructions in interrupt vector, each one is 4 bytes long

    uint32_t old_cfg = r_pmpcfg3();
    uint32_t new_cfg = old_cfg & (0x0000ffff);
    new_cfg |= 0x8C008000; // write 0b10001100 10000000 to set TOR mode, exec-only, and lock bit on both pmpaddr
    w_pmpcfg3(new_cfg);
}


void dump_mpu()
{
    dbg_printf(DL_EMERG,
               "pmp config:\n");
    dbg_printf(DL_EMERG,
               "0: %x ", r_pmpcfg0());
    dbg_printf(DL_EMERG,
               "1: %x ", r_pmpcfg1());
    dbg_printf(DL_EMERG,
               "2: %x ", r_pmpcfg2());
    dbg_printf(DL_EMERG,
               "3: %x\n", r_pmpcfg3());

    dbg_printf(DL_EMERG,
               "pmp address regions:\n");
    dbg_printf(DL_EMERG,
               "0-1: %p -> %p\n", r_pmpaddr0() << 2, r_pmpaddr1() << 2);
    dbg_printf(DL_EMERG,
               "2-3: %p -> %p\n", r_pmpaddr2() << 2, r_pmpaddr3() << 2);
    dbg_printf(DL_EMERG,
               "4-5: %p -> %p\n", r_pmpaddr4() << 2, r_pmpaddr5() << 2);
    dbg_printf(DL_EMERG,
               "6-7: %p -> %p\n", r_pmpaddr6() << 2, r_pmpaddr7() << 2);
    dbg_printf(DL_EMERG,
               "8-9: %p -> %p\n", r_pmpaddr8() << 2, r_pmpaddr9() << 2);
    dbg_printf(DL_EMERG,
               "10-11: %p -> %p\n", r_pmpaddr10() << 2, r_pmpaddr11() << 2);
    dbg_printf(DL_EMERG,
               "12-13: %p -> %p\n", r_pmpaddr12() << 2, r_pmpaddr13() << 2);
    dbg_printf(DL_EMERG,
               "14-15: %p -> %p\n", r_pmpaddr14() << 2, r_pmpaddr15() << 2);
}
