/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

/**
 * Fast I/O interfaces for Teensy 4.0 (IMXRT1062DVL6A) / 4.1 (IMXRT1062DVJ6A)
 * These use GPIO functions instead of Direct Port Manipulation, as on AVR.
 */

#ifndef PWM
  #define PWM OUTPUT
#endif

#define READ(IO)                digitalRead(IO)
#define WRITE(IO,V)             digitalWrite(IO,V)

#define _GET_MODE(IO)           !is_output(IO)
#define _SET_MODE(IO,M)         pinMode(IO, M)
#define _SET_OUTPUT(IO)         pinMode(IO, OUTPUT)                               /*!< Output Push Pull Mode & GPIO_NOPULL   */

#define OUT_WRITE(IO,V)         do{ _SET_OUTPUT(IO); WRITE(IO,V); }while(0)

#define SET_INPUT(IO)           _SET_MODE(IO, INPUT)                              /*!< Input Floating Mode                   */
#define SET_INPUT_PULLUP(IO)    _SET_MODE(IO, INPUT_PULLUP)                       /*!< Input with Pull-up activation         */
#define SET_INPUT_PULLDOWN(IO)  _SET_MODE(IO, INPUT_PULLDOWN)                     /*!< Input with Pull-down activation       */
#define SET_OUTPUT(IO)          OUT_WRITE(IO, LOW)
#define SET_PWM(IO)             _SET_MODE(IO, PWM)

#define TOGGLE(IO)              OUT_WRITE(IO, !READ(IO))

#define IS_INPUT(IO)            !is_output(IO)
#define IS_OUTPUT(IO)           is_output(IO)

#define PWM_PIN(P)            digitalPinHasPWM(P)

// digitalRead/Write wrappers
#define extDigitalRead(IO)    digitalRead(IO)
#define extDigitalWrite(IO,V) digitalWrite(IO,V)
