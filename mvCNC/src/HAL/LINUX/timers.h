/**
 * Modern Vintage CNC Firmware
 *
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

/**
 * HAL timers for Linux X86_64
 */

#include <stdint.h>

// ------------------------
// Defines
// ------------------------

#define FORCE_INLINE __attribute__((always_inline)) inline

typedef uint32_t hal_timer_t;
#define HAL_TIMER_TYPE_MAX 0xFFFFFFFF

#define HAL_TIMER_RATE         ((SystemCoreClock) / 4)  // frequency of timers peripherals

#ifndef MF_TIMER_STEP
  #define MF_TIMER_STEP         0  // Timer Index for Stepper
#endif
#ifndef MF_TIMER_PULSE
  #define MF_TIMER_PULSE        MF_TIMER_STEP
#endif
#ifndef MF_TIMER_TEMP
  #define MF_TIMER_TEMP         1  // Timer Index for Temperature
#endif

#define TEMP_TIMER_RATE        1000000
#define TEMP_TIMER_FREQUENCY   1000 // temperature interrupt frequency

#define STEPPER_TIMER_RATE     HAL_TIMER_RATE   // frequency of stepper timer (HAL_TIMER_RATE / STEPPER_TIMER_PRESCALE)
#define STEPPER_TIMER_TICKS_PER_US ((STEPPER_TIMER_RATE) / 1000000) // stepper timer ticks per µs
#define STEPPER_TIMER_PRESCALE (CYCLES_PER_MICROSECOND / STEPPER_TIMER_TICKS_PER_US)

#define PULSE_TIMER_RATE       STEPPER_TIMER_RATE   // frequency of pulse timer
#define PULSE_TIMER_PRESCALE   STEPPER_TIMER_PRESCALE
#define PULSE_TIMER_TICKS_PER_US STEPPER_TIMER_TICKS_PER_US

#define ENABLE_STEPPER_DRIVER_INTERRUPT() HAL_timer_enable_interrupt(MF_TIMER_STEP)
#define DISABLE_STEPPER_DRIVER_INTERRUPT() HAL_timer_disable_interrupt(MF_TIMER_STEP)
#define STEPPER_ISR_ENABLED() HAL_timer_interrupt_enabled(MF_TIMER_STEP)

#define ENABLE_TEMPERATURE_INTERRUPT() HAL_timer_enable_interrupt(MF_TIMER_TEMP)
#define DISABLE_TEMPERATURE_INTERRUPT() HAL_timer_disable_interrupt(MF_TIMER_TEMP)

#ifndef HAL_STEP_TIMER_ISR
  #define HAL_STEP_TIMER_ISR()  extern "C" void TIMER0_IRQHandler()
#endif
#ifndef HAL_TEMP_TIMER_ISR
  #define HAL_TEMP_TIMER_ISR()  extern "C" void TIMER1_IRQHandler()
#endif

// PWM timer
#define HAL_PWM_TIMER
#define HAL_PWM_TIMER_ISR()   extern "C" void TIMER3_IRQHandler()
#define HAL_PWM_TIMER_IRQn

void HAL_timer_init();
void HAL_timer_start(const uint8_t timer_num, const uint32_t frequency);

void HAL_timer_set_compare(const uint8_t timer_num, const hal_timer_t compare);
hal_timer_t HAL_timer_get_compare(const uint8_t timer_num);
hal_timer_t HAL_timer_get_count(const uint8_t timer_num);
FORCE_INLINE static void HAL_timer_restrain(const uint8_t timer_num, const uint16_t interval_ticks) {
  const hal_timer_t mincmp = HAL_timer_get_count(timer_num) + interval_ticks;
  if (HAL_timer_get_compare(timer_num) < mincmp) HAL_timer_set_compare(timer_num, mincmp);
}

void HAL_timer_enable_interrupt(const uint8_t timer_num);
void HAL_timer_disable_interrupt(const uint8_t timer_num);
bool HAL_timer_interrupt_enabled(const uint8_t timer_num);

#define HAL_timer_isr_prologue(T)
#define HAL_timer_isr_epilogue(T)
