/**
 * Modern Vintage CNC Firmware
*/
#pragma once

/**
 * VORON Design v2 pin assignments
 * See https://github.com/mzbotreprap/VORON/blob/master/Firmware/mvCNC/pins_RAMPS_VORON.h
 */

#define BOARD_INFO_NAME "VORON Design v2"

#define MOSFET_C_PIN                          11

#include "pins_RAMPS.h"

//
// Heaters / Fans
//
#undef FAN_PIN
#define FAN_PIN                                5  // Using the pin for the controller fan since controller fan is always on.
#define CONTROLLER_FAN_PIN                     8

//
// Auto fans
//
#ifndef E0_AUTO_FAN_PIN
  #define E0_AUTO_FAN_PIN                      6  // Servo pin 6 for E3D Fan
#endif
#ifndef E1_AUTO_FAN_PIN
  #define E1_AUTO_FAN_PIN                      6  // Servo pin 6 for E3D Fan (same pin for both ATC tools since it's the same fan)
#endif

//
// LCDs and Controllers
//
#undef BEEPER_PIN
