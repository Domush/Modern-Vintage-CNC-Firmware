/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

/**
 * Creality v2.4.S1 (STM32F103RE / STM32F103RC) v101 as found in the Ender 7 board pin assignments
 */

#define BOARD_INFO_NAME      "Creality v2.4.S1 V101"
#define DEFAULT_MACHINE_NAME "Creality3D"

//
// Heaters
//
#define HEATER_BED_PIN                      PA15  // HOT BED

#include "pins_CREALITY_V4.h"
