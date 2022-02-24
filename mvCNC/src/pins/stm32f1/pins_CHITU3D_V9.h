/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

#define BOARD_INFO_NAME "Chitu3D V9"

#define Z_STOP_PIN                          PA14

#define Z2_ENABLE_PIN                       PF3
#define Z2_STEP_PIN                         PF5
#define Z2_DIR_PIN                          PF1
#define Z2_STOP_PIN                         PA13

#ifndef Z_MIN_PROBE_PIN
  #define Z_MIN_PROBE_PIN                   PG9
#endif
#ifndef FIL_RUNOUT2_PIN
  #define FIL_RUNOUT2_PIN                   PF13
#endif

#include "pins_CHITU3D_common.h"
