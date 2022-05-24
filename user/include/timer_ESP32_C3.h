#ifndef TIMER_ESP32_C3_H_
#define TIMER_ESP32_C3_H_

#include <stdint.h>

extern volatile uint32_t *timg_t0update;
extern volatile uint32_t *timg_0_conf;
extern uint32_t timer_conf_en_bit;

#define TIMER_LATCH() (*timg_t0update = 1)
#define TIMER_START() (*timg_0_conf |= timer_conf_en_bit)

void timer_init();
void timer_start();
void timer_reset();
uint64_t timer_counter_to_microseconds(int counter_value);
uint64_t timer_get();

#endif // TIMER_ESP32_C3_H_
