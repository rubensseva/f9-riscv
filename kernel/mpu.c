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

/* w_pmpaddri writes a pmp addr register based on the pmp entry i (1-16). */
void w_pmpaddri(int i, uint32_t data) {
    w_pmpaddrarr[i](data);
}

/* w_pmpcfgi writes a pmp cfg register based on the pmp entry i (1-16).
 * Its a bit complex since the cfg configuration is densly packed in
 * registers cfg0-cfg3. Four bytes in each cfg register correspond
 * to the configuration of the 16 pmp entries. */
void w_pmpcfgi_region(int i, uint8_t data) {
    // First, figure out what cfg register we should operate on
    void (*__w_cfg)(uint32_t);
    uint32_t (*__r_cfg)(void);
    if (i < 4) {
        __w_cfg = w_pmpcfg0;
        __r_cfg = r_pmpcfg0;
    } else if (i < 8) {
        __w_cfg = w_pmpcfg1;
        __r_cfg = r_pmpcfg1;
    } else if (i < 12) {
        __w_cfg = w_pmpcfg2;
        __r_cfg = r_pmpcfg2;
    } else {
        __w_cfg = w_pmpcfg3;
        __r_cfg = r_pmpcfg3;
    }

    // Get the shift for the correct byte in the cfg register
    int shift = (i % 4) * 8;
    // Create a mask for the data region we want to operate on in the chosen cfg
    uint32_t mask = 0xFF << shift;
    // Shift the input data
    uint32_t shifted_data = data << shift;
    // Get the previous data from the chosen cfg
    uint32_t prev_data = __r_cfg();
    // Clear out the old data in the region we want to operate on in the chosen cfg
    prev_data &= ~mask;
    // OR the prev data with the shifted data. Since the prev data has zeros in the
    // region we want to operate on, and the shifted data has zero everywhere else
    // than the region we want to operate on, this should have the effect of only
    // setting the bits in the cfg in the chosen region, leaving the rest untouched.
    __w_cfg(prev_data | shifted_data);
}

uint32_t r_pmpcfgi(int i) {
    // First, figure out what cfg register we should operate on
    uint32_t (*__r_cfg)(void);
    if (i < 4) {
        __r_cfg = r_pmpcfg0;
    } else if (i < 8) {
        __r_cfg = r_pmpcfg1;
    } else if (i < 12) {
        __r_cfg = r_pmpcfg2;
    } else {
        __r_cfg = r_pmpcfg3;
    }
    return __r_cfg();
}

uint8_t r_pmpcfgi_region(int i) {
    // Get the shift for the correct byte in the cfg register
    int shift = (i % 4) * 8;
    // Create a mask for the data region we want to operate on in the chosen cfg
    uint32_t mask = 0xFF << shift;
    // Get the previous data from the chosen cfg
    uint32_t data = r_pmpcfgi(i);
    // Clear out all other bits than the ones we care about
    data &= mask;
    // Shift so we get the value of the cfg region we care about
    data >>= shift;
    return data;
}


/* mpu_setup_region sets up pmp for an fpage.
 * n is the number for a range corresponding to two pmp entries. For example:
 *     n = 0 means pmp entries 0 and 1
 *     n = 3 means pmp entries 6 and 7 */
void mpu_setup_region(int n, fpage_t *fp)
{
    if (n > 7) {
        // REALLY SHOULDNT BE HERE
        return;
    }

    int reg_lower = n * 2;
    int reg_upper = reg_lower + 1;

    if (fp) {
        // TODO: Check if these address are properly aligned
        // Shift 2 to the right because pmpaddr registers encode
        // bits 33-2 of the address.
        w_pmpaddri(reg_lower, (FPAGE_BASE(fp) >> 2));
        w_pmpaddri(reg_upper, (FPAGE_END(fp) >> 2));

        // If region does not already have rwx, then set it
        /* if ((r_pmpcfgi_region(reg_lower) & 0x7) != 0x7) { */
        /*     w_pmpcfgi_region(reg_lower, (r_pmpcfgi_region(reg_lower) | 0x7)); */
        /* } */
        /* if ((r_pmpcfgi_region(reg_upper) & 0x7) != 0x7) { */
        /*     w_pmpcfgi_region(reg_upper, (r_pmpcfgi_region(reg_upper) | 0x7)); */
        /* } */
        /* uint32_t test = r_mstatus(); */
        /* interrupt_disable(); */
        /* interrupt_enable(); */

        if ((r_pmpcfgi_region(reg_upper) & 0x7) != 0x7) {
            w_pmpcfgi_region(reg_upper, (r_pmpcfgi_region(reg_upper) | 0x7));
        }
        if ((r_pmpcfgi_region(reg_upper) & 0x8) != 0x8) {
            /* uint32_t test = r_mstatus(); */
            interrupt_disable();
            interrupt_enable();
            w_pmpcfgi_region(reg_upper, (r_pmpcfgi_region(reg_upper) | 0x8));
        }

        /* uint8_t old_data = r_pmpcfgi_region(reg_upper); */
    } else {
        /* Clean MPU region */
        /* w_pmpcfgi_region(reg_lower, (r_pmpcfgi_region(reg_lower) & 0xF8)); */
        w_pmpcfgi_region(reg_upper, (r_pmpcfgi_region(reg_upper) & 0x08));
    }
}
