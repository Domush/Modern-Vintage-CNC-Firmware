/**
 * Modern Vintage CNC Firmware
 *
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

/**
 * HAL for Teensy 3.2 (MK20DX256)
 */

#define CPU_32_BIT

#include "../shared/Marduino.h"
#include "../shared/math_32bit.h"
#include "../shared/HAL_SPI.h"

#include "fastio.h"
#include "watchdog.h"

#include <stdint.h>

#define CPU_ST7920_DELAY_1 600
#define CPU_ST7920_DELAY_2 750
#define CPU_ST7920_DELAY_3 750

//#undef MOTHERBOARD
//#define MOTHERBOARD BOARD_TEENSY31_32

#define IS_32BIT_TEENSY 1
#define IS_TEENSY_31_32 1
#ifndef IS_TEENSY31
  #define IS_TEENSY32 1
#endif

#include "../../core/serial_hook.h"

#define Serial0 Serial
#define _DECLARE_SERIAL(X) \
  typedef ForwardSerial1Class<decltype(Serial##X)> DefaultSerial##X; \
  extern DefaultSerial##X MSerial##X
#define DECLARE_SERIAL(X) _DECLARE_SERIAL(X)

typedef ForwardSerial1Class<decltype(SerialUSB)> USBSerialType;
extern USBSerialType USBSerial;

#define _MSERIAL(X) MSerial##X
#define MSERIAL(X) _MSERIAL(X)

#if SERIAL_PORT == -1
  #define MYSERIAL1 USBSerial
#elif WITHIN(SERIAL_PORT, 0, 3)
  DECLARE_SERIAL(SERIAL_PORT);
  #define MYSERIAL1 MSERIAL(SERIAL_PORT)
#else
  #error "The required SERIAL_PORT must be from 0 to 3, or -1 for Native USB."
#endif

#define HAL_SERVO_LIB libServo

typedef int8_t pin_t;

#ifndef analogInputToDigitalPin
  #define analogInputToDigitalPin(p) ((p < 12U) ? (p) + 54U : -1)
#endif

#define CRITICAL_SECTION_START()  uint32_t primask = __get_PRIMASK(); __disable_irq()
#define CRITICAL_SECTION_END()    if (!primask) __enable_irq()
#define ISRS_ENABLED() (!__get_PRIMASK())
#define ENABLE_ISRS()  __enable_irq()
#define DISABLE_ISRS() __disable_irq()

inline void HAL_init() {}

// Clear the reset reason
void HAL_clear_reset_source();

// Get the reason for the reset
uint8_t HAL_get_reset_source();

void HAL_reboot();

FORCE_INLINE void _delay_ms(const int delay_ms) { delay(delay_ms); }

#pragma GCC diagnostic push
#if GCC_VERSION <= 50000
  #pragma GCC diagnostic ignored "-Wunused-function"
#endif

extern "C" int freeMemory();

#pragma GCC diagnostic pop

// ADC

void HAL_adc_init();

#define HAL_ADC_VREF         3.3
#define HAL_ADC_RESOLUTION  10
#define HAL_START_ADC(pin)  HAL_adc_start_conversion(pin)
#define HAL_READ_ADC()      HAL_adc_get_result()
#define HAL_ADC_READY()     true

#define HAL_ANALOG_SELECT(pin)

void HAL_adc_start_conversion(const uint8_t adc_pin);
uint16_t HAL_adc_get_result();

// PWM

inline void set_pwm_duty(const pin_t pin, const uint16_t v, const uint16_t=255, const bool=false) { analogWrite(pin, v); }

// Pin Map

#define GET_PIN_MAP_PIN(index) index
#define GET_PIN_MAP_INDEX(pin) pin
#define PARSED_PIN_INDEX(code, dval) parser.intval(code, dval)
