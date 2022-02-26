/**
 * Modern Vintage CNC Firmware
*/
#pragma once

#if NOT_TARGET(__STM32F1__, STM32F1xx, STM32F0xx)
  #error "Oops! Select a 'Malyan M300' board in 'Tools > Board.'"
#endif

#define BOARD_INFO_NAME "Malyan M300"

//
// EEPROM Emulation
//
#if NO_EEPROM_SELECTED
  #define FLASH_EEPROM_EMULATION
  #ifndef MVCNC_EEPROM_SIZE
    #define MVCNC_EEPROM_SIZE 0x800U  // 2KB
  #endif
#endif

//
// SD CARD SPI
//
#define SDSS                           SD_SS_PIN

//
// Timers
//
#define STEP_TIMER  6
#define TEMP_TIMER  7

//
// Limit Switches
//
#define X_MAX_PIN                           PC13
#define Y_MAX_PIN                           PC14
#define Z_MAX_PIN                           PC15
#define Z_MIN_PIN                           PB7

//
// Steppers
//
#define X_STEP_PIN                          PB14
#define X_DIR_PIN                           PB13
#define X_ENABLE_PIN                        PB10

#define Y_STEP_PIN                          PB12
#define Y_DIR_PIN                           PB11
#define Y_ENABLE_PIN                        PB10

#define Z_STEP_PIN                          PB2
#define Z_DIR_PIN                           PB1
#define Z_ENABLE_PIN                        PB10

#define E0_STEP_PIN                         PA7
#define E0_DIR_PIN                          PA6
#define E0_ENABLE_PIN                       PB0

//
// Temperature Sensors
//
#define TEMP_0_PIN                          PA0   // Analog Input (HOTEND0 thermistor)
#define TEMP_BED_PIN                        PA4   // Analog Input (BED thermistor)

//
// Heaters / Fans
//
#define HEATER_0_PIN                        PA1   // HOTEND0 MOSFET
#define HEATER_BED_PIN                      PA5   // BED MOSFET

#define AUTO_FAN_PIN                        PA8
