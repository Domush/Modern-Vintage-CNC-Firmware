/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

/**
 * Fast I/O interfaces for STM32
 * These use GPIO register access for fast port manipulation.
 */

// ------------------------
// Public Variables
// ------------------------

extern GPIO_TypeDef * FastIOPortMap[];

// ------------------------
// Public functions
// ------------------------

void FastIO_init(); // Must be called before using fast io macros
#define FASTIO_INIT() FastIO_init()

// ------------------------
// Defines
// ------------------------

#define _BV32(b) (1UL << (b))

#ifndef PWM
  #define PWM OUTPUT
#endif

#if defined(STM32F0xx) || defined(STM32F1xx) || defined(STM32F3xx) || defined(STM32L0xx) || defined(STM32L4xx)
  #define _WRITE(IO, V) do { \
    if (V) FastIOPortMap[STM_PORT(digitalPinToPinName(IO))]->BSRR = _BV32(STM_PIN(digitalPinToPinName(IO))) ; \
    else   FastIOPortMap[STM_PORT(digitalPinToPinName(IO))]->BRR  = _BV32(STM_PIN(digitalPinToPinName(IO))) ; \
  }while(0)
#else
  #define _WRITE(IO, V) (FastIOPortMap[STM_PORT(digitalPinToPinName(IO))]->BSRR = _BV32(STM_PIN(digitalPinToPinName(IO)) + ((V) ? 0 : 16)))
#endif

#define _READ(IO)               bool(READ_BIT(FastIOPortMap[STM_PORT(digitalPinToPinName(IO))]->IDR, _BV32(STM_PIN(digitalPinToPinName(IO)))))
#define _TOGGLE(IO)             TBI32(FastIOPortMap[STM_PORT(digitalPinToPinName(IO))]->ODR, STM_PIN(digitalPinToPinName(IO)))

#define _GET_MODE(IO)
#define _SET_MODE(IO,M)         pinMode(IO, M)
#define _SET_OUTPUT(IO)         pinMode(IO, OUTPUT)                               //!< Output Push Pull Mode & GPIO_NOPULL
#define _SET_OUTPUT_OD(IO)      pinMode(IO, OUTPUT_OPEN_DRAIN)

#define WRITE(IO,V)             _WRITE(IO,V)
#define READ(IO)                _READ(IO)
#define TOGGLE(IO)              _TOGGLE(IO)

#define OUT_WRITE(IO,V)         do{ _SET_OUTPUT(IO); WRITE(IO,V); }while(0)
#define OUT_WRITE_OD(IO,V)      do{ _SET_OUTPUT_OD(IO); WRITE(IO,V); }while(0)

#define SET_INPUT(IO)           _SET_MODE(IO, INPUT)                              //!< Input Floating Mode
#define SET_INPUT_PULLUP(IO)    _SET_MODE(IO, INPUT_PULLUP)                       //!< Input with Pull-up activation
#define SET_INPUT_PULLDOWN(IO)  _SET_MODE(IO, INPUT_PULLDOWN)                     //!< Input with Pull-down activation
#define SET_OUTPUT(IO)          OUT_WRITE(IO, LOW)
#define SET_PWM(IO)             _SET_MODE(IO, PWM)

#define IS_INPUT(IO)
#define IS_OUTPUT(IO)

#define PWM_PIN(P)              digitalPinHasPWM(P)
#define NO_COMPILE_TIME_PWM

// digitalRead/Write wrappers
#define extDigitalRead(IO)    digitalRead(IO)
#define extDigitalWrite(IO,V) digitalWrite(IO,V)