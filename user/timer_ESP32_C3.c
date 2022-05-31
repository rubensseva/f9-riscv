#include <ESP32_C3.h>
#include <utility.h>
#include <link.h>

#include <stdint.h>
#include <timer_ESP32_C3.h>


__USER_DATA volatile uint32_t *timg_0_update = REG(TIMER_GROUP_0_BASE + TIMG_T0UPDATE_REG);
__USER_DATA volatile uint32_t *timg_0_conf = REG(TIMER_GROUP_0_BASE + TIMG_T0CONFIG_REG);

__USER_DATA uint32_t timer_conf_en_bit = (1 << 31);

__USER_TEXT void timer_init() {
    volatile uint32_t *timg_0_conf = REG(TIMER_GROUP_0_BASE + TIMG_T0CONFIG_REG);

    /* Make sure the timer is stopped */
    *timg_0_conf &= ~(1 << 31);

    /* Select clock source by setting or clearing TIMG_TO_USE_XTAL field */
    *timg_0_conf |= (1 << 9); // Choose XTAL_CLK as source, which is 40Mhz

    /* Configure the 16-bit prescaler by setting TIMG_TO_DIVIDER */
    *timg_0_conf &= ~(0xffff << 13); // Clear divider field in conf register
    *timg_0_conf |= (10000 << 13); // Set prescaler field to 10000. This gives 4000Hz if the source clock is XTAL_CLK.
    *timg_0_conf |= (1 << 12); // Reset prescaler (needs to be done for prescaler to work, according to TRM section 11.2.1)

    /* Configure the timer direction by setting or clearing TIMG_TO_INCREASE */
    *timg_0_conf |= (1 << 30); // Set direction upwards

    /* Set the timer's starting value by writing the starting value to TIMG_TO_LOAD_LO and
       TIMG_TO_LOAD_HI, then reloading it into the timer by writing any value to TIMG_TOLOAD_REG */
    volatile uint32_t *timg_t0loadlo = REG(TIMER_GROUP_0_BASE + TIMG_T0LOADLO_REG);
    volatile uint32_t *timg_t0loadhi = REG(TIMER_GROUP_0_BASE + TIMG_T0LOADHI_REG);
    *timg_t0loadlo = *timg_t0loadhi = 0;
    volatile uint32_t *timg_t0load = REG(TIMER_GROUP_0_BASE + TIMG_T0LOAD_REG);
    *timg_t0load = 1;
}


/* Stop the timer and reset its value */
__USER_TEXT void timer_reset() {
    volatile uint32_t *timg_0_conf = REG(TIMER_GROUP_0_BASE + TIMG_T0CONFIG_REG);
    *timg_0_conf &= ~(1 << 31);
    volatile uint32_t *timg_t0load = REG(TIMER_GROUP_0_BASE + TIMG_T0LOAD_REG);
    *timg_t0load = 1;
}

__USER_TEXT uint64_t timer_counter_to_microseconds(int counter_value) {
    int clk_freq = 4000; // 4000 Hz
    int useconds_per_cycle = 1000000 / clk_freq;
    return counter_value * useconds_per_cycle;
}

/* Read the timer value. Remember to latch the timer first! */
__USER_TEXT uint64_t timer_get() {
    volatile uint32_t *timg_t0lo = REG(TIMER_GROUP_0_BASE + TIMG_T0LO_REG);
    volatile uint32_t *timg_t0hi = REG(TIMER_GROUP_0_BASE + TIMG_T0HI_REG);
    uint64_t res = 0;
    res = *timg_t0lo;
    res |= ((uint64_t) (*timg_t0hi)) << 32;
    return res;
}
