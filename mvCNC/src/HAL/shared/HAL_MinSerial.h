/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

#include "../../core/serial.h"
#include <stdint.h>

// Serial stuff here
// Inside an exception handler, the CPU state is not safe, we can't expect the handler to resume
// and the software to continue. UART communication can't rely on later callback/interrupt as it might never happen.
// So, you need to provide some method to send one byte to the usual UART with the interrupts disabled
// By default, the method uses SERIAL_CHAR but it's 100% guaranteed to break (couldn't be worse than nothing...)7
extern void (*HAL_min_serial_init)();
extern void (*HAL_min_serial_out)(char ch);

struct MinSerial {
  static bool force_using_default_output;
  // Serial output
  static void TX(char ch) {
    if (force_using_default_output)
      SERIAL_CHAR(ch);
    else
      HAL_min_serial_out(ch);
  }
  // Send String through UART
  static void TX(const char *s) { while (*s) TX(*s++); }
  // Send a digit through UART
  static void TXDigit(uint32_t d) {
    if (d < 10) TX((char)(d+'0'));
    else if (d < 16) TX((char)(d+'A'-10));
    else TX('?');
  }

  // Send Hex number through UART
  static void TXHex(uint32_t v) {
    TX("0x");
    for (uint8_t i = 0; i < 8; i++, v <<= 4)
      TXDigit((v >> 28) & 0xF);
  }

  // Send Decimal number through UART
  static void TXDec(uint32_t v) {
    if (!v) {
      TX('0');
      return;
    }

    char nbrs[14];
    char *p = &nbrs[0];
    while (v != 0) {
      *p++ = '0' + (v % 10);
      v /= 10;
    }
    do {
      p--;
      TX(*p);
    } while (p != &nbrs[0]);
  }
  static void init() { if (!force_using_default_output) HAL_min_serial_init(); }
};
