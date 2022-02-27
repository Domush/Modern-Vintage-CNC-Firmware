/**
 * Modern Vintage CNC Firmware
*/
#pragma once

/**
 * Creality v4.5.3 (STM32F103RE / STM32F103RC) board pin assignments
 */

#if TOOL_CHANGE_SUPPORT || E_STEPPERS > 1
  #error "Creality v4.5.3 only supports one hotend / E-stepper. Comment out this line to continue."
#endif

#define BOARD_INFO_NAME "Creality v4.5.3"

#define HEATER_0_PIN                        PB14  // HEATER1
#define HEATER_BED_PIN                      PB13  // HOT BED
#define FAN_PIN                             PB15  // FAN

#if ENABLED(PROBE_ACTIVATION_SWITCH)
  #ifndef PROBE_ACTIVATION_SWITCH_PIN
    #define PROBE_ACTIVATION_SWITCH_PIN     PB2   // Optoswitch to Enable Z Probe
  #endif
#endif

#include "pins_CREALITY_V45x.h"
