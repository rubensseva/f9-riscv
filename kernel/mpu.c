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

// FIXME: Needs adjustment for 32 bit

/* w_pmpaddri writes a pmp addr register based on the pmp entry i (1-16). */
void w_pmpaddri(int i, uint32_t data) {
    w_pmpaddrarr[i](data);
}

/* w_pmpcfgi writes a pmp cfg register based on the pmp entry i (1-16).
 * Its a bit complex since the cfg configuration is densly packed in
 * cfg0 and cfg2. Eight bytes in cfg0 and eight bytes in cfg2 correspond
 * to the configuration of the 16 pmp entries. */
void w_pmpcfgi(int i, uint32_t data) {
    void (*w_func)(uint32_t);
    uint32_t (*r_func)(void);
    if (i < 7) {
        w_func = w_pmpcfg0;
        r_func = r_pmpcfg0;
    } else {
        w_func = w_pmpcfg2;
        r_func = r_pmpcfg2;
    }

    int shift = (i % 8);
    uint32_t mask = 0xF << shift;
    uint32_t shifted_data = data << shift;
    uint32_t cfg_data = r_func();
    cfg_data &= ~mask;
    w_func(cfg_data | shifted_data);
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

    w_pmpaddri(reg_lower, FPAGE_BASE(fp));
    w_pmpaddri(reg_upper, FPAGE_END(fp));

    if (fp) {
        // write 0b1111 to cfgs to allow rwx and enable TOR mode
        w_pmpcfgi(reg_lower, 15);
        w_pmpcfgi(reg_upper, 15);

        w_pmpaddri(reg_lower, FPAGE_BASE(fp));
        w_pmpaddri(reg_upper, FPAGE_END(fp));

    } else {
        /* Clean MPU region */
        w_pmpcfgi(reg_lower, 0x0);
        w_pmpcfgi(reg_upper, 0x0);
    }
}
