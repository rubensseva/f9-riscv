#include "mpu.h"
#include "riscv.h"


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

/* w_pmpaddri writes a pmp addr register based on the pmp entry (1-16). */
void w_pmpaddri(int pmp_entry, uint32_t data) {
    w_pmpaddrarr[pmp_entry](data);
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
    if (n > 7) {
        return;
    }
    int pmp_entry_lower = n * 2;
    int pmp_entry_upper = pmp_entry_lower + 1;
    uint32_t old_cfg_data = r_pmpcfgi(pmp_entry_upper);

    if (fp) {
        w_pmpaddri(pmp_entry_lower, (FPAGE_BASE(fp) >> 2));
        w_pmpaddri(pmp_entry_upper, (FPAGE_END(fp) >> 2));

        // Check if we need to write to cfg, perform write if necessary
        int shift = (pmp_entry_upper % 4) * 8;
        uint32_t mask = 0xFF << shift;
        uint32_t masked = old_cfg_data & mask;
        uint32_t shifted = masked >> shift;
        if ((shifted & 0xF) != 0xF) {
            w_pmpcfgi_region(pmp_entry_upper, 0xF, old_cfg_data);
        }
    } else {
        // Clear region
        w_pmpcfgi_region(pmp_entry_upper, 0x0, old_cfg_data);
    }
}
