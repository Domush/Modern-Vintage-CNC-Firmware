/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

#include "../config/DGUS_Addr.h"

#define VPFLAG_NONE        0
#define VPFLAG_AUTOUPLOAD  (1U << 0) // Upload on every DGUS update
#define VPFLAG_RXSTRING    (1U << 1) // Treat the received data as a string (terminated with 0xFFFF)

struct DGUS_VP {
  DGUS_Addr addr;
  uint8_t   size;
  uint8_t   flags;
  void      *extra;

  // Callback that will be called if the display modified the value.
  // nullptr makes it readonly for the display.
  void (*rx_handler)(DGUS_VP &, void *);
  void (*tx_handler)(DGUS_VP &);
};
