/**
 * Modern Vintage CNC Firmware
*/
#pragma once

 /**
  *  ╦╔═╗╔═╗┬ ┬┬─┐┌─┐┬─┐┌─┐╔═╗┌─┐┬─┐┬ ┬┌┬┐ ┌─┐┌─┐┌┬┐
  *  ║║ ╦╠═╣│ │├┬┘│ │├┬┘├─┤╠╣ │ │├┬┘│ ││││ │  │ ││││
  * ╚╝╚═╝╩ ╩└─┘┴└─└─┘┴└─┴ ┴╚  └─┘┴└─└─┘┴ ┴o└─┘└─┘┴ ┴
  *   Pin assignments for 32-bit JGAurora A5S & A1
  *
  * https://jgaurorawiki.com/_media/jgaurora_a5s_a1_pinout.png
  */

#include "env_validate.h"

#if TOOL_CHANGE_SUPPORT || E_STEPPERS > 1
  #error "JGAurora A5S A1 only supports one hotend / E-stepper. Comment out this line to continue."
#endif

#define BOARD_INFO_NAME "JGAurora A5S A1"

#define BOARD_NO_NATIVE_USB

#ifndef STM32_XL_DENSITY
  #define STM32_XL_DENSITY
#endif

//#define MCU_STM32F103ZE // not yet required

// Enable EEPROM Emulation for this board, so that we don't overwrite factory data
#if NO_EEPROM_SELECTED
  //#define I2C_EEPROM                            // AT24C64
  //#define FLASH_EEPROM_EMULATION
#endif

#if ENABLED(I2C_EEPROM)
//#define MVCNC_EEPROM_SIZE          0x8000UL  // 32KB
#elif ENABLED(FLASH_EEPROM_EMULATION)
//#define MVCNC_EEPROM_SIZE          0x1000UL  // 4KB
//#define MVCNC_EEPROM_SIZE (EEPROM_START_ADDRESS + (EEPROM_PAGE_SIZE) * 2UL)
#endif

//
// Limit Switches
//
#define X_STOP_PIN                          PC6
#define Y_STOP_PIN                          PG8
#define Z_STOP_PIN                          PG7
//#define X_MAX_PIN                         PC5
//#define Y_MAX_PIN                         PC4
//#define Z_MAX_PIN                         PB0

//
// Steppers
//
#define X_STEP_PIN                          PD6
#define X_DIR_PIN                           PD3
#define X_ENABLE_PIN                        PG9

#define Y_STEP_PIN                          PG12
#define Y_DIR_PIN                           PG11
#define Y_ENABLE_PIN                        PG13

#define Z_STEP_PIN                          PG15
#define Z_DIR_PIN                           PG14
#define Z_ENABLE_PIN                        PB8

#define E0_STEP_PIN                         PE2
#define E0_DIR_PIN                          PB9
#define E0_ENABLE_PIN                       PE3

#define E1_STEP_PIN                         PE5
#define E1_DIR_PIN                          PE4
#define E1_ENABLE_PIN                       PE6

//
// Temperature Sensors
//
#define TEMP_0_PIN                          PC2
#define TEMP_BED_PIN                        PC1

//
// Heaters / Fans
//
#define HEATER_0_PIN                        PA2
#define HEATER_BED_PIN                      PA3

#define FAN_PIN                             PA1

#define FIL_RUNOUT_PIN                      PC7

//
// TFT with FSMC interface
//
#if HAS_FSMC_TFT
  #define LCD_BACKLIGHT_PIN                 PF11
  #define FSMC_CS_PIN                       PD7
  #define FSMC_RS_PIN                       PG0

  #define LCD_USE_DMA_FSMC                        // Use DMA transfers to send data to the TFT
  #define FSMC_DMA_DEV                      DMA2
  #define FSMC_DMA_CHANNEL               DMA_CH5

  #define TFT_CS_PIN                 FSMC_CS_PIN
  #define TFT_RS_PIN                 FSMC_RS_PIN
#endif

//
// SD Card
//
#define SD_DETECT_PIN                       PF10

//
// Misc.
//
#define BEEPER_PIN                          PC3   // use PB7 to shut up if desired
#define LED_PIN                             PC13

//
// Touch support
//
#if NEED_TOUCH_PINS
  #define TOUCH_CS_PIN                      PA4
  #define TOUCH_INT_PIN                     PC4
  #define TOUCH_MISO_PIN                    PA6
  #define TOUCH_MOSI_PIN                    PA7
  #define TOUCH_SCK_PIN                     PA5
#endif