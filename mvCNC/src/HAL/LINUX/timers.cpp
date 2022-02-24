/**
 * Modern Vintage CNC Firmware
 *
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#ifdef __PLAT_LINUX__

#include "hardware/Timer.h"

#include "../../inc/mvCNCConfig.h"

/**
 * Use POSIX signals to attempt to emulate Interrupts
 * This has many limitations and is not fit for the purpose
 */

HAL_STEP_TIMER_ISR();
HAL_TEMP_TIMER_ISR();

Timer timers[2];

void HAL_timer_init() {
  timers[0].init(0, STEPPER_TIMER_RATE, TIMER0_IRQHandler);
  timers[1].init(1, TEMP_TIMER_RATE, TIMER1_IRQHandler);
}

void HAL_timer_start(const uint8_t timer_num, const uint32_t frequency) {
  timers[timer_num].start(frequency);
}

void HAL_timer_enable_interrupt(const uint8_t timer_num) {
  timers[timer_num].enable();
}

void HAL_timer_disable_interrupt(const uint8_t timer_num) {
  timers[timer_num].disable();
}

bool HAL_timer_interrupt_enabled(const uint8_t timer_num) {
  return timers[timer_num].enabled();
}

void HAL_timer_set_compare(const uint8_t timer_num, const hal_timer_t compare) {
  timers[timer_num].setCompare(compare);
}

hal_timer_t HAL_timer_get_compare(const uint8_t timer_num) {
  return timers[timer_num].getCompare();
}

hal_timer_t HAL_timer_get_count(const uint8_t timer_num) {
  return timers[timer_num].getCount();
}

#endif // __PLAT_LINUX__
