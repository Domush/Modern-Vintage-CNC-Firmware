/**
 * Modern Vintage CNC Firmware
*/
#pragma once

/**
 * FYSETC E4 pin assignments
 *
 * CNC control board based on the ESP32 microcontroller.
 * Supports 4 stepper drivers, heated bed, single hotend.
 */

#include "env_validate.h"

#if ATC_TOOLS > 1 || E_STEPPERS > 1
  #error "FYSETC E4 only supports one E Stepper. Comment out this line to continue."
#elif TOOL_CHANGE_SUPPORT
  #error "FYSETC E4 only supports one hotend / E-stepper. Comment out this line to continue."
#endif

#define BOARD_INFO_NAME       "FYSETC_E4"

#include "pins_ESPA_common.h"

#if HAS_TMC_UART
  //
  // TMC2209 stepper drivers
  //

  //
  // Hardware serial 1
  //
  #define X_HARDWARE_SERIAL              Serial1
  #define Y_HARDWARE_SERIAL              Serial1
  #define Z_HARDWARE_SERIAL              Serial1
  #define E0_HARDWARE_SERIAL             Serial1

  #define TMC_BAUD_RATE 115200
#endif

/**
 * Hardware serial pins
 *
 * Override these pins in Configuration.h or Configuration_adv.h if needed.
 *
 * Note: Serial2 can be defined using HARDWARE_SERIAL2_RX and HARDWARE_SERIAL2_TX
 *       but MRR ESPA does not have enough spare pins for such reassignment.
 */
#ifndef HARDWARE_SERIAL1_RX
  #define HARDWARE_SERIAL1_RX                 21
#endif
#ifndef HARDWARE_SERIAL1_TX
  #define HARDWARE_SERIAL1_TX                 22
#endif
