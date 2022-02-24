/**
 * Modern Vintage CNC Firmware
 *
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

#define _useTimer1
#define _useTimer2

#define TRIM_DURATION           5   // compensation ticks to trim adjust for digitalWrite delays
#define SERVO_TIMER_PRESCALER   64  // timer prescaler factor to 64 (avoid overflowing 16-bit clock counter, at 120MHz this is 1831 ticks per millisecond

#define SERVO_TC                3

typedef enum {
  #ifdef _useTimer1
    _timer1,
  #endif
  #ifdef _useTimer2
    _timer2,
  #endif
  _Nbr_16timers
} timer16_Sequence_t;
