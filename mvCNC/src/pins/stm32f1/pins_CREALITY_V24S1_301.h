/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

/**
 * Creality V24S1_301 (STM32F103RE / STM32F103RC) board pin assignments as found on Ender 3 S1
 */

#include "env_validate.h"

#if HAS_MULTI_HOTEND || E_STEPPERS > 1
  #error "Creality V4 only supports one hotend / E-stepper. Comment out this line to continue."
#endif

#define BOARD_INFO_NAME      "Creality V24S1-301"
#define DEFAULT_MACHINE_NAME "Ender 3 S1"

//
// Servos
//
#if !defined(SERVO0_PIN) && !defined(HAS_PIN_27_BOARD)
  #define SERVO0_PIN                        PC13  // BLTouch OUT
#endif

//
// Limit Switches
//
#define Z_STOP_PIN                          PC14

#ifndef Z_MIN_PROBE_PIN
  #define Z_MIN_PROBE_PIN                   PC14  // BLTouch IN
#endif

//
// Filament Runout Sensor
//
#ifndef FIL_RUNOUT_PIN
  #define FIL_RUNOUT_PIN                    PC15  // "Pulled-high"
#endif

//
// Heaters / Fans
//
#define HEATER_BED_PIN                      PA7   // HOT BED
#define FAN1_PIN                            PC0   // extruder fan

#include "pins_CREALITY_V4.h"
