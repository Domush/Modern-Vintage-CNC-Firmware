/**
 * Modern Vintage CNC Firmware
*/
#pragma once

/**
 * Creality v4.5.2 (STM32F103RE / STM32F103RC) board pin assignments
 */

#if TOOL_CHANGE_SUPPORT || E_STEPPERS > 1
  #error "Creality v4.5.2 only supports one hotend / E-stepper. Comment out this line to continue."
#endif

#define BOARD_INFO_NAME "Creality v4.5.2"

#define HEATER_0_PIN                        PA1   // HEATER1
#define HEATER_BED_PIN                      PA2   // HOT BED
#define FAN_PIN                             PA0   // FAN

#if ENABLED(PROBE_ACTIVATION_SWITCH)
  #ifndef PROBE_ACTIVATION_SWITCH_PIN
    #define PROBE_ACTIVATION_SWITCH_PIN     PC6   // Optoswitch to Enable Z Probe
  #endif
#endif

#include "pins_CREALITY_V45x.h"
