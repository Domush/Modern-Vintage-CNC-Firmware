/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

/**
 * HAL Timers for Teensy 4.0 (IMXRT1062DVL6A) / 4.1 (IMXRT1062DVJ6A)
 */

#include <stdint.h>

// ------------------------
// Defines
// ------------------------

#define FORCE_INLINE __attribute__((always_inline)) inline

typedef uint32_t hal_timer_t;
#define HAL_TIMER_TYPE_MAX 0xFFFFFFFE

#define GPT_TIMER_RATE F_BUS_ACTUAL   // 150MHz

#define GPT1_TIMER_PRESCALE 2
#define GPT2_TIMER_PRESCALE 10

#define GPT1_TIMER_RATE (GPT_TIMER_RATE / GPT1_TIMER_PRESCALE) // 75MHz
#define GPT2_TIMER_RATE (GPT_TIMER_RATE / GPT2_TIMER_PRESCALE) // 15MHz

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
#define TEMP_TIMER_FREQUENCY   1000

#define STEPPER_TIMER_RATE     GPT1_TIMER_RATE
#define STEPPER_TIMER_TICKS_PER_US ((STEPPER_TIMER_RATE) / 1000000)
#define STEPPER_TIMER_PRESCALE ((GPT_TIMER_RATE / 1000000) / STEPPER_TIMER_TICKS_PER_US)

#define PULSE_TIMER_RATE       STEPPER_TIMER_RATE   // frequency of pulse timer
#define PULSE_TIMER_PRESCALE   STEPPER_TIMER_PRESCALE
#define PULSE_TIMER_TICKS_PER_US STEPPER_TIMER_TICKS_PER_US

#define ENABLE_STEPPER_DRIVER_INTERRUPT() HAL_timer_enable_interrupt(MF_TIMER_STEP)
#define DISABLE_STEPPER_DRIVER_INTERRUPT() HAL_timer_disable_interrupt(MF_TIMER_STEP)
#define STEPPER_ISR_ENABLED() HAL_timer_interrupt_enabled(MF_TIMER_STEP)

#define ENABLE_TEMPERATURE_INTERRUPT() HAL_timer_enable_interrupt(MF_TIMER_TEMP)
#define DISABLE_TEMPERATURE_INTERRUPT() HAL_timer_disable_interrupt(MF_TIMER_TEMP)

#ifndef HAL_STEP_TIMER_ISR
  #define HAL_STEP_TIMER_ISR() extern "C" void stepTC_Handler() // GPT1_Handler()
#endif
#ifndef HAL_TEMP_TIMER_ISR
  #define HAL_TEMP_TIMER_ISR() extern "C" void tempTC_Handler() // GPT2_Handler()
#endif

extern "C" {
  void stepTC_Handler();
  void tempTC_Handler();
}

void HAL_timer_start(const uint8_t timer_num, const uint32_t frequency);

FORCE_INLINE static void HAL_timer_set_compare(const uint8_t timer_num, const hal_timer_t compare) {
  switch (timer_num) {
    case MF_TIMER_STEP: GPT1_OCR1 = compare - 1; break;
    case MF_TIMER_TEMP: GPT2_OCR1 = compare - 1; break;
  }
}

FORCE_INLINE static hal_timer_t HAL_timer_get_compare(const uint8_t timer_num) {
  switch (timer_num) {
    case MF_TIMER_STEP: return GPT1_OCR1;
    case MF_TIMER_TEMP: return GPT2_OCR1;
  }
  return 0;
}

FORCE_INLINE static hal_timer_t HAL_timer_get_count(const uint8_t timer_num) {
  switch (timer_num) {
    case MF_TIMER_STEP: return GPT1_CNT;
    case MF_TIMER_TEMP: return GPT2_CNT;
  }
  return 0;
}

void HAL_timer_enable_interrupt(const uint8_t timer_num);
void HAL_timer_disable_interrupt(const uint8_t timer_num);
bool HAL_timer_interrupt_enabled(const uint8_t timer_num);

void HAL_timer_isr_prologue(const uint8_t timer_num);
//void HAL_timer_isr_epilogue(const uint8_t timer_num) {}
#define HAL_timer_isr_epilogue(T)