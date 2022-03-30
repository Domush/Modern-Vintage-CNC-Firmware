/**
 * Modern Vintage CNC Firmware
 */

/**
 * The purpose of this file is just include mvCNC Configuration files,
 * to discover which FEATURES are enabled, without any HAL include.
 * Used by common-dependencies.py
 */

#include "../../../../mvCNC/src/inc/mvCNCConfig.h"

//
// Conditionals only used for [features]
//
#if ENABLED(SR_LCD_3W_NL)
  // Feature checks for SR_LCD_3W_NL
#elif EITHER(LCD_I2C_TYPE_MCP23017, LCD_I2C_TYPE_MCP23008)
  #define USES_LIQUIDTWI2
#elif ENABLED(LCD_I2C_TYPE_PCA8574)
  #define USES_LIQUIDCRYSTAL_I2C
#elif ANY(HAS_MVCNCUI_HD44780, LCD_I2C_TYPE_PCF8575, SR_LCD_2W_NL, LCM1602)
  #define USES_LIQUIDCRYSTAL
#endif

#if SAVED_POSITIONS
  #define HAS_SAVED_POSITIONS
#endif

#if ENABLED(HOST_PROMPT_SUPPORT) && DISABLED(EMERGENCY_PARSER)
  #define HAS_GCODE_M876
#endif

#if ENABLED(DUET_SMART_EFFECTOR) && PIN_EXISTS(SMART_EFFECTOR_MOD)
  #define HAS_SMART_EFF_MOD
#endif

#if HAS_MVCNCUI_MENU
  #if ENABLED(BACKLASH_GCODE)
    #define HAS_MENU_BACKLASH
  #endif
  #if ENABLED(LEVEL_BED_CORNERS)
    #define HAS_MENU_BED_CORNERS
  #endif
  #if ENABLED(CANCEL_OBJECTS)
    #define HAS_MENU_CANCELOBJECT
  #endif
  #if EITHER(DELTA_CALIBRATION_MENU, DELTA_AUTO_CALIBRATION)
    #define HAS_MENU_DELTA_CALIBRATE
  #endif
  #if EITHER(LED_CONTROL_MENU, CASE_LIGHT_MENU)
    #define HAS_MENU_LED
  #endif
  #if ENABLED(ADVANCED_PAUSE_FEATURE)
    #define HAS_MENU_FILAMENT
  #endif
  #if ENABLED(SDSUPPORT)
    #define HAS_MENU_MEDIA
  #endif
  #if ENABLED(MIXING_EXTRUDER)
    #define HAS_MENU_MIXER
  #endif
  #if ENABLED(POWER_LOSS_RECOVERY)
    #define HAS_MENU_JOB_RECOVERY
  #endif
  #if HAS_POWER_MONITOR
    #define HAS_MENU_POWER_MONITOR
  #endif
  #if HAS_CUTTER
    #define HAS_MENU_CUTTER
  #endif
  #if HAS_TEMPERATURE
    #define HAS_MENU_TEMPERATURE
  #endif
  #if ENABLED(MMU2_MENUS)
    #define HAS_MENU_MMU2
  #endif
  #if ENABLED(PASSWORD_FEATURE)
    #define HAS_MENU_PASSWORD
  #endif
  #if HAS_TRINAMIC_CONFIG
    #define HAS_MENU_TMC
  #endif
  #if ENABLED(TOUCH_SCREEN_CALIBRATION)
    #define HAS_MENU_TOUCH_SCREEN
  #endif
  #if ENABLED(ASSISTED_TRAMMING_WIZARD)
    #define HAS_MENU_TRAMMING
  #endif
  #if ENABLED(AUTO_BED_LEVELING_UBL)
    #define HAS_MENU_UBL
  #endif
#endif
