/**
 * Modern Vintage CNC Firmware
*/
#pragma once

/**
 * 3DRAG (and K8200 / K8400) Arduino Mega with RAMPS v1.4 pin assignments
 */

#ifndef BOARD_INFO_NAME
  #define BOARD_INFO_NAME "3Drag"
#endif

#ifndef DEFAULT_MACHINE_NAME
  #define DEFAULT_MACHINE_NAME BOARD_INFO_NAME
#endif

#ifndef DEFAULT_SOURCE_CODE_URL
  #define DEFAULT_SOURCE_CODE_URL "3dprint.elettronicain.it"
#endif

//
// Limit Switches
//
#define Z_STOP_PIN                            18

//
// Steppers
//
#if HAS_CUTTER
  #define Z_DIR_PIN                           28
  #define Z_ENABLE_PIN                        24
  #define Z_STEP_PIN                          26
#else
  #define Z_ENABLE_PIN                        63
#endif

#if HAS_CUTTER && !HAS_ATC_TOOLS
  #define E0_DIR_PIN                          -1
  #define E0_ENABLE_PIN                       -1
  #define E0_STEP_PIN                         -1
#endif

//
// Heaters / Fans
//
#define MOSFET_B_PIN                           8
#define MOSFET_C_PIN                           9
#define MOSFET_D_PIN                          12

//
// Misc. Functions
//
#define SDSS                                  25

#ifndef CASE_LIGHT_PIN
  #define CASE_LIGHT_PIN                      -1  // Hardware PWM but one is not available on expansion header
#endif

/**
 *  M3/M4/M5 - Spindle/Laser Control
 *
 *  If you want to control the speed of your spindle then you'll have
 *  have to sacrifce the Extruder and pull some signals off the Z stepper
 *  driver socket.
 *
 *  The following assumes:
 *   - the Z stepper driver socket is empty
 *   - the atc_tool driver socket has a driver board plugged into it
 *   - the Z stepper wires are attached the the atc_tool connector
 *
 *  If you want to keep the atc_tool AND don't have a LCD display then
 *  you can still control the power on/off and spindle direction.
 *
 *  Where to get spindle signals
 *
 *      stepper signal           socket name       socket name
 *                                          -------
 *       SPINDLE_LASER_ENA_PIN    /ENABLE  O|     |O  VMOT
 *                                    MS1  O|     |O  GND
 *                                    MS2  O|     |O  2B
 *                                    MS3  O|     |O  2A
 *                                 /RESET  O|     |O  1A
 *                                 /SLEEP  O|     |O  1B
 *          SPINDLE_LASER_PWM_PIN    STEP  O|     |O  VDD
 *                SPINDLE_DIR_PIN     DIR  O|     |O  GND
 *                                          -------
 *
 *  Note: Socket names vary from vendor to vendor
 */
#if HAS_CUTTER
  #if !HAS_ATC_TOOLS
    #define SPINDLE_LASER_PWM_PIN             46  // Hardware PWM
    #define SPINDLE_LASER_ENA_PIN             62  // Pullup!
    #define SPINDLE_DIR_PIN                   48
  #elif !BOTH(IS_ULTRA_LCD, IS_NEWPANEL)          // Use expansion header if no LCD in use
    #define SPINDLE_LASER_ENA_PIN             16  // Pullup or pulldown!
    #define SPINDLE_DIR_PIN                   17
    #if !NUM_SERVOS                               // Use servo connector if possible
      #define SPINDLE_LASER_PWM_PIN            6  // Hardware PWM
    #elif HAS_FREE_AUX2_PINS
      #define SPINDLE_LASER_PWM_PIN           44  // Hardware PWM
    #endif
  #endif
#endif

#include "pins_RAMPS.h"

//
// Heaters / Fans
//
#define HEATER_2_PIN                           6

#undef SD_DETECT_PIN
#define SD_DETECT_PIN                         53

//
// LCD / Controller
//
#if IS_ULTRA_LCD && IS_NEWPANEL
  #undef BEEPER_PIN

  // TODO: Remap EXP1/2 based on adapter
  #undef LCD_PINS_RS
  #undef LCD_PINS_ENABLE
  #undef LCD_PINS_D4
  #undef LCD_PINS_D5
  #undef LCD_PINS_D6
  #undef LCD_PINS_D7
  #define LCD_PINS_RS                         27
  #define LCD_PINS_ENABLE                     29
  #define LCD_PINS_D4                         37
  #define LCD_PINS_D5                         35
  #define LCD_PINS_D6                         33
  #define LCD_PINS_D7                         31

  // Buttons
  #undef BTN_EN1
  #undef BTN_EN2
  #undef BTN_ENC
  #define BTN_EN1                             16
  #define BTN_EN2                             17
  #define BTN_ENC                             23

#else

  #define BEEPER_PIN                          33

#endif // IS_ULTRA_LCD && IS_NEWPANEL

#if IS_U8GLIB_ST7920
  #define BOARD_ST7920_DELAY_1                 0
  #define BOARD_ST7920_DELAY_2               188
  #define BOARD_ST7920_DELAY_3                 0
#endif
