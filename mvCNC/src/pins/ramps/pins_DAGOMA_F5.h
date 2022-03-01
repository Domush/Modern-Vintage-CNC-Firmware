/**
 * Modern Vintage CNC Firmware
*/
#pragma once

#if HOTENDS > 2 || E_STEPPERS > 2
  #error "Dagoma3D F5 supports only 2 hotends / E-steppers. Comment out this line to continue."
#endif

#define BOARD_INFO_NAME "Dagoma3D F5"

//
// Endstops
//
#define X_STOP_PIN                             2
#define Y_STOP_PIN                             3
#define Z_STOP_PIN                            15

#define FIL_RUNOUT_PIN                        39
#if ATC_TOOLS > 1
  #define FIL_RUNOUT2_PIN                     14
#endif

// Alter timing for graphical display
#if IS_U8GLIB_ST7920
  #define BOARD_ST7920_DELAY_1                 0
  #define BOARD_ST7920_DELAY_2               250
  #define BOARD_ST7920_DELAY_3               250
#endif

//
// DAC steppers
//
#define HAS_MOTOR_CURRENT_DAC 1

#define DAC_STEPPER_ORDER { 0, 1, 2, 3 }

#define DAC_STEPPER_SENSE    0.11
#define DAC_STEPPER_ADDRESS  0
#define DAC_STEPPER_MAX      4096
#define DAC_STEPPER_VREF     1
#define DAC_STEPPER_GAIN     0
#define DAC_OR_ADDRESS       0x00

//
// Import default RAMPS 1.4 pins
//
#include "pins_RAMPS.h"
