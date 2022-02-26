/**
 * Modern Vintage CNC Firmware
*/
#pragma once

/**
 * No official schematics have been found.
 * But these differences where noted in https://github.com/bigtreetech/Rumba32/issues/1
 */

#define BOARD_INFO_NAME "RUMBA32 (BTT)"

#if NO_EEPROM_SELECTED
  #define I2C_EEPROM
  #define MVCNC_EEPROM_SIZE 0x1000  // 4KB (24LC32AT-I/OT)
#endif

#if ENABLED(FLASH_EEPROM_EMULATION)
  // Decrease delays and flash wear by spreading writes across the
  // 128 kB sector allocated for EEPROM emulation.
  #define FLASH_EEPROM_LEVELING
#endif

#include "pins_RUMBA32_common.h"

#define SERVO0_PIN                          PA15  // Pin is not broken out, is a test point only.

#if HAS_TMC_UART
  //
  // TMC2208/TMC2209 Software Serial
  //
  #define X_SERIAL_TX_PIN                   PC14  // BTT Rumba32 only uses 1 pin for UART
  #define X_SERIAL_RX_PIN        X_SERIAL_TX_PIN

  #define Y_SERIAL_TX_PIN                   PE4
  #define Y_SERIAL_RX_PIN        Y_SERIAL_TX_PIN

  #define Z_SERIAL_TX_PIN                   PE0
  #define Z_SERIAL_RX_PIN        Z_SERIAL_TX_PIN

  #define E0_SERIAL_TX_PIN                  PC13
  #define E0_SERIAL_RX_PIN      E0_SERIAL_TX_PIN

  #define E1_SERIAL_TX_PIN                  PD5
  #define E1_SERIAL_RX_PIN      E1_SERIAL_TX_PIN

  #define E2_SERIAL_TX_PIN                  PD1
  #define E2_SERIAL_RX_PIN      E2_SERIAL_TX_PIN
#endif
