/**
 * Modern Vintage CNC Firmware
*/
#pragma once

/**
 * MRR ESPA pin assignments
 *
 * CNC control board based on the ESP32 microcontroller.
 * Supports 4 stepper drivers, heated bed, single hotend.
 */

#include "env_validate.h"

#if ATC_TOOLS > 1 || E_STEPPERS > 1
  #error "MRR ESPA only supports one E Stepper. Comment out this line to continue."
#elif TOOL_CHANGE_SUPPORT
  #error "MRR ESPA only supports one hotend / E-stepper. Comment out this line to continue."
#endif

#define BOARD_INFO_NAME       "MRR ESPA"
#define BOARD_WEBSITE_URL     "github.com/maplerainresearch/MRR_ESPA"

#include "pins_ESPA_common.h"

//
// Steppers
//
//#define X_CS_PIN                            21
//#define Y_CS_PIN                            22
//#define Z_CS_PIN                             5  // SS_PIN
//#define E0_CS_PIN                           21

// Hardware serial pins
// Add the following to Configuration.h or Configuration_adv.h to assign
// specific pins to hardware Serial1.
// Note: Serial2 can be defined using HARDWARE_SERIAL2_RX and HARDWARE_SERIAL2_TX but
// MRR ESPA does not have enough spare pins for such reassignment.
//#define HARDWARE_SERIAL1_RX                 21
//#define HARDWARE_SERIAL1_TX                 22
