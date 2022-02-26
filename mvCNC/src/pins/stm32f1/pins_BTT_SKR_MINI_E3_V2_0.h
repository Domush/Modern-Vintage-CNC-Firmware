/**
 * Modern Vintage CNC Firmware
*/
#pragma once

#define SKR_MINI_E3_V2

#define BOARD_CUSTOM_BUILD_FLAGS -DTONE_CHANNEL=4 -DTONE_TIMER=4 -DTIMER_TONE=4

// Onboard I2C EEPROM
#if NO_EEPROM_SELECTED
  #define I2C_EEPROM
  #define SOFT_I2C_EEPROM
  #define MVCNC_EEPROM_SIZE                0x1000  // 4KB
  #define I2C_SDA_PIN                      PB7
  #define I2C_SCL_PIN                      PB6
  #undef NO_EEPROM_SELECTED
#endif

#include "pins_BTT_SKR_MINI_E3_common.h"

#ifndef BOARD_INFO_NAME
  #define BOARD_INFO_NAME "BTT SKR Mini E3 V2.0"
#endif

// Release PA13/PA14 (led, usb control) from SWD pins
#define DISABLE_DEBUG

#ifndef NEOPIXEL_PIN
  #define NEOPIXEL_PIN                     PA8   // LED driving pin
#endif

#ifndef PS_ON_PIN
  #define PS_ON_PIN                        PC13  // Power Supply Control
#endif

#define FAN1_PIN                           PC7

#ifndef CONTROLLER_FAN_PIN
  #define CONTROLLER_FAN_PIN               FAN1_PIN
#endif

#if HAS_TMC_UART
  /**
   * TMC220x stepper drivers
   * Hardware serial communication ports
   */
  #define X_HARDWARE_SERIAL  MSerial4
  #define Y_HARDWARE_SERIAL  MSerial4
  #define Z_HARDWARE_SERIAL  MSerial4
  #define E0_HARDWARE_SERIAL MSerial4

  // Default TMC slave addresses
  #ifndef X_SLAVE_ADDRESS
    #define X_SLAVE_ADDRESS  0
  #endif
  #ifndef Y_SLAVE_ADDRESS
    #define Y_SLAVE_ADDRESS  2
  #endif
  #ifndef Z_SLAVE_ADDRESS
    #define Z_SLAVE_ADDRESS  1
  #endif
  #ifndef E0_SLAVE_ADDRESS
    #define E0_SLAVE_ADDRESS 3
  #endif
#endif
