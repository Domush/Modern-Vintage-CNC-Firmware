/**
 * Modern Vintage CNC Firmware
*/
#pragma once

/**
 * RAMPS-FD v2
 *
 * EEPROM supported
 * Use 1k thermistor tables
 */

#define BOARD_INFO_NAME "RAMPS-FD v2"

#ifndef E0_CS_PIN
  #define E0_CS_PIN        69 // moved from A13 to A15 on v2.2, if not earlier
#endif

#include "pins_RAMPS_FD_V1.h"

#undef INVERTED_HEATER_PINS
#undef INVERTED_BED_PINS
#undef INVERTED_FAN_PINS

#define I2C_EEPROM
#define MVCNC_EEPROM_SIZE 0x10000  // 64K in a 24C512

#ifndef PS_ON_PIN
  #define PS_ON_PIN        12
#endif

#ifndef FILWIDTH_PIN
  #define FILWIDTH_PIN      5   // Analog Input on AUX2
#endif
