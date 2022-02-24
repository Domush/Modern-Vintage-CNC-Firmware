/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

/**
 * CREALITY v4.2.3 (STM32F103RE / STM32F103RC) board pin assignments
 */

#define BOARD_INFO_NAME      "Creality v4.2.3"
#define DEFAULT_MACHINE_NAME "Creality3D"

//
// Heaters
//
#define HEATER_BED_PIN                      PB10  // HOT BED

#include "pins_CREALITY_V4.h"

//
// Encoder
//
#if BTN_EN1 == PB10
  #undef BTN_EN1
  #define BTN_EN1                           PA2   // Rotary Encoder
#endif
