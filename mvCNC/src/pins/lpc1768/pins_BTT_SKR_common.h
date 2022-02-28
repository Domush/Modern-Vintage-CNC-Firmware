/**
 * Modern Vintage CNC Firmware
*/
#pragma once

#include "env_validate.h"

// If you have the BigTreeTech driver expansion module, enable BTT_MOTOR_EXPANSION
// https://github.com/bigtreetech/BTT-Expansion-module/tree/master/BTT%20EXP-MOT
//#define BTT_MOTOR_EXPANSION

#if BOTH(HAS_WIRED_LCD, BTT_MOTOR_EXPANSION)
  #if EITHER(CR10_STOCKDISPLAY, ENDER2_STOCKDISPLAY)
    #define EXP_MOT_USE_EXP2_ONLY 1
  #else
    #error "You can't use both an LCD and a Motor Expansion Module on EXP1/EXP2 at the same time."
  #endif
#endif

// Ignore temp readings during development.
//#define BOGUS_TEMPERATURE_GRACE_PERIOD    2000

//
// Steppers
//
#ifndef E1_STEP_PIN
  #define E1_STEP_PIN                      P0_01
#endif
#ifndef E1_DIR_PIN
  #define E1_DIR_PIN                       P0_00
#endif
#ifndef E1_ENABLE_PIN
  #define E1_ENABLE_PIN                    P0_10
#endif

//
// Fans
//
#ifndef FAN_PIN
  #define FAN_PIN                        P2_03
#endif
#ifndef FAN1_PIN
  #define FAN1_PIN                       P2_04
#endif

//
// LCD / Controller
//
#if !defined(BEEPER_PIN) && HAS_WIRED_LCD && DISABLED(LCD_USE_I2C_BUZZER)
  #define BEEPER_PIN                       P1_30  // (37) not 5V tolerant
#endif

//
// SD Support
//
#ifndef SDCARD_CONNECTION
  #if HAS_WIRED_LCD
    #define SDCARD_CONNECTION                LCD
  #else
    #define SDCARD_CONNECTION            ONBOARD
  #endif
#endif

#if SD_CONNECTION_IS(LCD) && ENABLED(SKR_USE_LCD_SD_CARD_PINS_FOR_CS)
  #error "SDCARD_CONNECTION must not be 'LCD' with SKR_USE_LCD_SD_CARD_PINS_FOR_CS."
#endif

#if SD_CONNECTION_IS(LCD)
  #define SD_SCK_PIN                       P0_15
  #define SD_MISO_PIN                      P0_17
  #define SD_MOSI_PIN                      P0_18
  #define SD_SS_PIN                  EXP2_07_PIN
  #define SD_DETECT_PIN              EXP2_04_PIN

#elif SD_CONNECTION_IS(ONBOARD)
  #undef SD_DETECT_PIN
  #define SD_DETECT_PIN                    P0_27
  #define SD_SCK_PIN                       P0_07
  #define SD_MISO_PIN                      P0_08
  #define SD_MOSI_PIN                      P0_09
  #define ONBOARD_SD_CS_PIN                P0_06  // Chip select for "System" SD card
  #define SD_SS_PIN            ONBOARD_SD_CS_PIN
#elif SD_CONNECTION_IS(CUSTOM_CABLE)
  #error "No custom SD drive cable defined for this board."
#endif

#if ENABLED(BTT_MOTOR_EXPANSION)
  /**
   *          ------                       ------
   * (M3STP) |10  9 | (M3DIR)    (M3DIAG) |10  9 | (M3RX)
   * (M2STP) | 8  7 | (M2DIR)    (M2DIAG) | 8  7 | (M2RX)
   * (M1DIR)   6  5 | (M1STP)    (M1DIAG)   6  5 | (M1RX)
   *  (M1EN) | 4  3 | --           (M3EN) | 4  3 | (M2EN)
   *     GND | 2  1 | --              GND | 2  1 | --
   *          ------                       ------
   *           EXP2                         EXP1
   *
   * NB In EXP_MOT_USE_EXP2_ONLY mode EXP1 is not used and M2EN and M3EN need to be jumpered to M1EN
   */

  // M1 on Driver Expansion Module
  #define E2_STEP_PIN                EXP2_05_PIN
  #define E2_DIR_PIN                 EXP2_06_PIN
  #define E2_ENABLE_PIN              EXP2_04_PIN
  #if !EXP_MOT_USE_EXP2_ONLY
    #define E2_DIAG_PIN              EXP1_06_PIN
    #define E2_CS_PIN                EXP1_05_PIN
    #if HAS_TMC_UART
      #define E2_SERIAL_TX_PIN       EXP1_05_PIN
      #define E2_SERIAL_RX_PIN  E2_SERIAL_TX_PIN
    #endif
  #endif

  // M2 on Driver Expansion Module
  #define E3_STEP_PIN                EXP2_08_PIN
  #define E3_DIR_PIN                 EXP2_07_PIN
  #if !EXP_MOT_USE_EXP2_ONLY
    #define E3_ENABLE_PIN            EXP1_03_PIN
    #define E3_DIAG_PIN              EXP1_08_PIN
    #define E3_CS_PIN                EXP1_07_PIN
    #if HAS_TMC_UART
      #define E3_SERIAL_TX_PIN       EXP1_07_PIN
      #define E3_SERIAL_RX_PIN  E3_SERIAL_TX_PIN
    #endif
  #else
    #define E3_ENABLE_PIN            EXP2_04_PIN
  #endif

  // M3 on Driver Expansion Module
  #define E4_STEP_PIN                EXP2_10_PIN
  #define E4_DIR_PIN                 EXP2_09_PIN
  #if !EXP_MOT_USE_EXP2_ONLY
    #define E4_ENABLE_PIN            EXP1_04_PIN
    #define E4_DIAG_PIN              EXP1_10_PIN
    #define E4_CS_PIN                EXP1_09_PIN
    #if HAS_TMC_UART
      #define E4_SERIAL_TX_PIN       EXP1_09_PIN
      #define E4_SERIAL_RX_PIN  E4_SERIAL_TX_PIN
    #endif
  #else
    #define E4_ENABLE_PIN            EXP2_04_PIN
  #endif

#endif // BTT_MOTOR_EXPANSION
