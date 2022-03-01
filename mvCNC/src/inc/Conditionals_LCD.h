/**
 * Modern Vintage CNC Firmware
*/
#pragma once

/**
 * Conditionals_LCD.h
 * Conditionals that need to be set before Configuration_adv.h or pins.h
 */

// MKS_LCD12864A/B is a variant of MKS_MINI_12864
#if EITHER(MKS_LCD12864A, MKS_LCD12864B)
  #define MKS_MINI_12864
#endif

// MKS_MINI_12864_V3 and BTT_MINI_12864_V1 are identical to FYSETC_MINI_12864_2_1
#if EITHER(MKS_MINI_12864_V3, BTT_MINI_12864_V1)
  #define FYSETC_MINI_12864_2_1
#endif

/**
 * General Flags that may be set below by specific LCDs
 *
 *  DOGLCD                  : Run a Graphical LCD through U8GLib (with mvCNCUI)
 *  IS_ULTIPANEL            : Define LCD_PINS_D5/6/7 for direct-connected "Ultipanel" LCDs
 *  IS_ULTRA_LCD            : Ultra LCD, not necessarily Ultipanel.
 *  IS_RRD_SC               : Common RRD Smart Controller digital interface pins
 *  IS_RRD_FG_SC            : Common RRD Full Graphical Smart Controller digital interface pins
 *  IS_U8GLIB_ST7920        : Most common DOGM display SPI interface, supporting a "lightweight" display mode.
 *  U8GLIB_SH1106           : SH1106 OLED with I2C interface via U8GLib
 *  IS_U8GLIB_SSD1306       : SSD1306 OLED with I2C interface via U8GLib (U8GLIB_SSD1306)
 *  U8GLIB_SSD1309          : SSD1309 OLED with I2C interface via U8GLib (HAS_U8GLIB_I2C_OLED, IS_ULTRA_LCD, DOGLCD)
 *  IS_U8GLIB_ST7565_64128N : ST7565 128x64 LCD with SPI interface via U8GLib
 *  IS_U8GLIB_LM6059_AF     : LM6059 with Hardware SPI via U8GLib
 */
#if EITHER(MKS_MINI_12864, ENDER2_STOCKDISPLAY)

  #define MINIPANEL

#elif ENABLED(YHCB2004)

  #define IS_ULTIPANEL 1

#elif ENABLED(CARTESIO_UI)

  #define DOGLCD
  #define IS_ULTIPANEL 1

#elif EITHER(DWIN_mvCNCUI_PORTRAIT, DWIN_mvCNCUI_LANDSCAPE)

  #define IS_DWIN_MVCNCUI 1
  #define IS_ULTIPANEL 1

#elif ENABLED(ZONESTAR_LCD)

  #define HAS_ADC_BUTTONS 1
  #define REPRAPWORLD_KEYPAD_MOVE_STEP 10.0
  #define ADC_KEY_NUM 8
  #define IS_ULTIPANEL 1

  // This helps to implement HAS_ADC_BUTTONS menus
  #define REVERSE_MENU_DIRECTION
  #define STD_ENCODER_PULSES_PER_STEP 1
  #define STD_ENCODER_STEPS_PER_MENU_ITEM 1
  #define ENCODER_FEEDRATE_DEADZONE 2

#elif ENABLED(ZONESTAR_12864LCD)
  #define DOGLCD
  #define IS_RRD_SC 1
  #define IS_U8GLIB_ST7920 1

#elif ENABLED(ZONESTAR_12864OLED)
  #define IS_RRD_SC 1
  #define U8GLIB_SH1106

#elif ENABLED(ZONESTAR_12864OLED_SSD1306)
  #define IS_RRD_SC 1
  #define IS_U8GLIB_SSD1306

#elif ENABLED(RADDS_DISPLAY)
  #define IS_ULTIPANEL 1
  #define STD_ENCODER_PULSES_PER_STEP 2

#elif ANY(miniVIKI, VIKI2, WYH_L12864, ELB_FULL_GRAPHIC_CONTROLLER, AZSMZ_12864)

  #define IS_DOGM_12864 1

  #define DOGLCD
  #define IS_ULTIPANEL 1

  #if ENABLED(miniVIKI)
    #define IS_U8GLIB_ST7565_64128N 1
  #elif ENABLED(VIKI2)
    #define IS_U8GLIB_ST7565_64128N 1
  #elif ENABLED(WYH_L12864)
    #define IS_U8GLIB_ST7565_64128N 1
    #define ST7565_XOFFSET 0x04
  #elif ENABLED(ELB_FULL_GRAPHIC_CONTROLLER)
    #define IS_U8GLIB_LM6059_AF 1
  #elif ENABLED(AZSMZ_12864)
    #define IS_U8GLIB_ST7565_64128N 1
  #endif

#elif ENABLED(OLED_PANEL_TINYBOY2)

  #define IS_U8GLIB_SSD1306
  #define IS_ULTIPANEL 1

#elif ENABLED(RA_CONTROL_PANEL)

  #define LCD_I2C_TYPE_PCA8574
  #define LCD_I2C_ADDRESS 0x27   // I2C Address of the port expander
  #define IS_ULTIPANEL 1

#elif ENABLED(REPRAPWORLD_GRAPHICAL_LCD)

  #define DOGLCD
  #define IS_U8GLIB_ST7920 1
  #define IS_ULTIPANEL 1

#elif ENABLED(MKS_12864OLED)

  #define IS_RRD_SC 1
  #define U8GLIB_SH1106

#elif ENABLED(MKS_12864OLED_SSD1306)

  #define IS_RRD_SC 1
  #define IS_U8GLIB_SSD1306

#elif ENABLED(SAV_3DGLCD)

  #ifdef U8GLIB_SSD1306
    #define IS_U8GLIB_SSD1306 // Allow for U8GLIB_SSD1306 + SAV_3DGLCD
  #endif
  #define IS_NEWPANEL 1

#elif ENABLED(FYSETC_242_OLED_12864)

  #define IS_RRD_SC 1
  #define U8GLIB_SH1106

  #ifndef NEOPIXEL_BRIGHTNESS
    #define NEOPIXEL_BRIGHTNESS 127
  #endif

#elif ANY(FYSETC_MINI_12864_X_X, FYSETC_MINI_12864_1_2, FYSETC_MINI_12864_2_0, FYSETC_MINI_12864_2_1, FYSETC_GENERIC_12864_1_1)

  #define FYSETC_MINI_12864
  #define DOGLCD
  #define IS_ULTIPANEL 1
  #define LED_COLORS_REDUCE_GREEN

  // Require LED backlighting enabled
  #if ENABLED(FYSETC_MINI_12864_2_1)
    #ifndef NEOPIXEL_BRIGHTNESS
      #define NEOPIXEL_BRIGHTNESS 127
    #endif
    //#define NEOPIXEL_STARTUP_TEST
  #endif

#elif ENABLED(ULTI_CONTROLLER)

  #define IS_ULTIPANEL 1
  #define U8GLIB_SSD1309
  #define LCD_RESET_PIN LCD_PINS_D6 //  This controller need a reset pin
  #define STD_ENCODER_PULSES_PER_STEP 4
  #define STD_ENCODER_STEPS_PER_MENU_ITEM 1
  #ifndef PCA9632
    #define PCA9632
  #endif

#elif ENABLED(MAKEBOARD_MINI_2_LINE_DISPLAY_1602)

  #define IS_RRD_SC 1
  #define LCD_WIDTH 16
  #define LCD_HEIGHT 2

#elif EITHER(TFTGLCD_PANEL_SPI, TFTGLCD_PANEL_I2C)

  #define IS_TFTGLCD_PANEL 1
  #define IS_ULTIPANEL 1                    // Note that IS_ULTIPANEL leads to HAS_WIRED_LCD

  #if ENABLED(SDSUPPORT) && DISABLED(LCD_PROGRESS_BAR)
    #define LCD_PROGRESS_BAR
  #endif
  #if ENABLED(TFTGLCD_PANEL_I2C)
    #define LCD_I2C_ADDRESS           0x33  // Must be 0x33 for STM32 main boards and equal to panel's I2C slave address
  #endif
  #define LCD_USE_I2C_BUZZER                // Enable buzzer on LCD, used for both I2C and SPI buses (LiquidTWI2 not required)
  #define STD_ENCODER_PULSES_PER_STEP 2
  #define STD_ENCODER_STEPS_PER_MENU_ITEM 1
  #define LCD_WIDTH                   20    // 20 or 24 chars in line
  #define LCD_HEIGHT                  10    // Character lines
  #define LCD_CONTRAST_MIN            127
  #define LCD_CONTRAST_MAX            255
  #define LCD_CONTRAST_DEFAULT        250
  #define CONVERT_TO_EXT_ASCII        // Use extended 128-255 symbols from ASCII table.
                                      // At this time present conversion only for cyrillic - bg, ru and uk languages.
                                      // First 7 ASCII symbols in panel font must be replaced with mvCNC's special symbols.

#elif ENABLED(CR10_STOCKDISPLAY)

  #define IS_RRD_FG_SC 1
  #define LCD_ST7920_DELAY_1           125
  #define LCD_ST7920_DELAY_2           125
  #define LCD_ST7920_DELAY_3           125

#elif EITHER(ANET_FULL_GRAPHICS_LCD, ANET_FULL_GRAPHICS_LCD_ALT_WIRING)

  #define IS_RRD_FG_SC 1
  #define LCD_ST7920_DELAY_1           150
  #define LCD_ST7920_DELAY_2           150
  #define LCD_ST7920_DELAY_3           150

#elif ANY(REPRAP_DISCOUNT_FULL_GRAPHIC_SMART_CONTROLLER, BQ_LCD_SMART_CONTROLLER, K3D_FULL_GRAPHIC_SMART_CONTROLLER)

  #define IS_RRD_FG_SC 1

#elif ENABLED(REPRAP_DISCOUNT_SMART_CONTROLLER)

  #define IS_RRD_SC 1   // RepRapDiscount LCD or Graphical LCD with rotary click encoder

#elif ENABLED(K3D_242_OLED_CONTROLLER)

  #define IS_RRD_SC 1
  #define U8GLIB_SSD1309

#endif

// ST7920-based graphical displays
#if ANY(IS_RRD_FG_SC, LCD_FOR_MELZI, SILVER_GATE_GLCD_CONTROLLER)
  #define DOGLCD
  #define IS_U8GLIB_ST7920 1
  #define IS_RRD_SC 1
#endif

// ST7565 / 64128N graphical displays
#if EITHER(MAKRPANEL, MINIPANEL)
  #define IS_ULTIPANEL 1
  #define DOGLCD
  #if ENABLED(MAKRPANEL)
    #define IS_U8GLIB_ST7565_64128N 1
  #endif
#endif

#if ENABLED(IS_U8GLIB_SSD1306)
  #define U8GLIB_SSD1306
#endif

#if ENABLED(OVERLORD_OLED)
  #define IS_ULTIPANEL 1
  #define U8GLIB_SH1106
  /**
   * PCA9632 for buzzer and LEDs via i2c
   * No auto-inc, red and green leds switched, buzzer
   */
  #define PCA9632
  #define PCA9632_NO_AUTO_INC
  #define PCA9632_GRN         0x00
  #define PCA9632_RED         0x02
  #define PCA9632_BUZZER
  #define PCA9632_BUZZER_DATA { 0x09, 0x02 }

  #define STD_ENCODER_PULSES_PER_STEP     1 // Overlord uses buttons
  #define STD_ENCODER_STEPS_PER_MENU_ITEM 1
#endif

// 128x64 I2C OLED LCDs - SSD1306/SSD1309/SH1106
#if ANY(U8GLIB_SSD1306, U8GLIB_SSD1309, U8GLIB_SH1106)
  #define HAS_U8GLIB_I2C_OLED 1
  #define IS_ULTRA_LCD 1
  #define DOGLCD
#endif

/**
 * SPI Ultipanels
 */

// Basic Ultipanel-like displays
#if ANY(ULTIMAKERCONTROLLER, IS_RRD_SC, G3D_PANEL, RIGIDBOT_PANEL, PANEL_ONE, U8GLIB_SH1106)
  #define IS_ULTIPANEL 1
#endif

// Einstart OLED has Cardinal nav via pins defined in pins_EINSTART-S.h
#if ENABLED(U8GLIB_SH1106_EINSTART)
  #define DOGLCD
  #define IS_ULTIPANEL 1
#endif

// TFT Compatibility
#if ANY(FSMC_GRAPHICAL_TFT, SPI_GRAPHICAL_TFT, TFT_320x240, TFT_480x320, TFT_320x240_SPI, TFT_480x320_SPI, TFT_LVGL_UI_FSMC, TFT_LVGL_UI_SPI)
  #define IS_LEGACY_TFT 1
  #define TFT_GENERIC
#endif

#if ANY(FSMC_GRAPHICAL_TFT, TFT_320x240, TFT_480x320, TFT_LVGL_UI_FSMC)
  #define TFT_INTERFACE_FSMC
#elif ANY(SPI_GRAPHICAL_TFT, TFT_320x240_SPI, TFT_480x320_SPI, TFT_LVGL_UI_SPI)
  #define TFT_INTERFACE_SPI
#endif

#if EITHER(FSMC_GRAPHICAL_TFT, SPI_GRAPHICAL_TFT)
  #define TFT_CLASSIC_UI
#elif ANY(TFT_320x240, TFT_480x320, TFT_320x240_SPI, TFT_480x320_SPI)
  #define TFT_COLOR_UI
#elif EITHER(TFT_LVGL_UI_FSMC, TFT_LVGL_UI_SPI)
  #define TFT_LVGL_UI
#endif

// FSMC/SPI TFT Panels (LVGL)
#if ENABLED(TFT_LVGL_UI)
  #define HAS_TFT_LVGL_UI 1
  #define SERIAL_RUNTIME_HOOK 1
#endif

// FSMC/SPI TFT Panels
#if ENABLED(TFT_CLASSIC_UI)
  #define TFT_SCALED_DOGLCD 1
#endif

#if TFT_SCALED_DOGLCD
  #define DOGLCD
  #define IS_ULTIPANEL 1
  #define DELAYED_BACKLIGHT_INIT
#elif HAS_TFT_LVGL_UI
  #define DELAYED_BACKLIGHT_INIT
#endif

// Color UI
#if ENABLED(TFT_COLOR_UI)
  #define HAS_GRAPHICAL_TFT 1
  #define IS_ULTIPANEL 1
#endif

/**
 * I2C Panels
 */

#if ANY(IS_RRD_SC, IS_DOGM_12864, OLED_PANEL_TINYBOY2, LCD_I2C_PANELOLU2)

  #define STD_ENCODER_PULSES_PER_STEP 4
  #define STD_ENCODER_STEPS_PER_MENU_ITEM 1

  #if ENABLED(LCD_I2C_PANELOLU2)  // PANELOLU2 LCD with status LEDs, separate encoder and click inputs
    #define LCD_I2C_TYPE_MCP23017 // I2C Character-based 12864 display
    #define LCD_I2C_ADDRESS 0x20  // I2C Address of the port expander
    #define LCD_USE_I2C_BUZZER    // Enable buzzer on LCD (optional)
    #define IS_ULTIPANEL 1
  #endif

#elif EITHER(LCD_SAINSMART_I2C_1602, LCD_SAINSMART_I2C_2004)

  #define LCD_I2C_TYPE_PCF8575    // I2C Character-based 12864 display
  #define LCD_I2C_ADDRESS 0x27    // I2C Address of the port expander

  #if ENABLED(LCD_SAINSMART_I2C_2004)
    #define LCD_WIDTH 20
    #define LCD_HEIGHT 4
  #endif

#elif ENABLED(LCD_I2C_VIKI)

  /**
   * Panucatt VIKI LCD with status LEDs, integrated click & L/R/U/P buttons, separate encoder inputs
   *
   * This uses the LiquidTWI2 library v1.2.3 or later ( https://github.com/lincomatic/LiquidTWI2 )
   * Make sure the LiquidTWI2 directory is placed in the Arduino or Sketchbook libraries subdirectory.
   * Note: The pause/stop/resume LCD button pin should be connected to the Arduino
   *       BTN_ENC pin (or set BTN_ENC to -1 if not used)
   */
  #define LCD_I2C_TYPE_MCP23017
  #define LCD_I2C_ADDRESS 0x20 // I2C Address of the port expander
  #define LCD_USE_I2C_BUZZER   // Enable buzzer on LCD (requires LiquidTWI2 v1.2.3 or later)
  #define IS_ULTIPANEL 1

  #define ENCODER_FEEDRATE_DEADZONE 4

  #define STD_ENCODER_PULSES_PER_STEP 1
  #define STD_ENCODER_STEPS_PER_MENU_ITEM 2

#elif ENABLED(G3D_PANEL)

  #define STD_ENCODER_PULSES_PER_STEP 2
  #define STD_ENCODER_STEPS_PER_MENU_ITEM 1

#endif

#if EITHER(LCD_I2C_TYPE_MCP23017, LCD_I2C_TYPE_MCP23008) && DISABLED(NO_LCD_DETECT)
  #define DETECT_I2C_LCD_DEVICE 1
#endif

#ifndef STD_ENCODER_PULSES_PER_STEP
  #if ENABLED(TOUCH_SCREEN)
    #define STD_ENCODER_PULSES_PER_STEP 2
  #else
    #define STD_ENCODER_PULSES_PER_STEP 5
  #endif
#endif
#ifndef STD_ENCODER_STEPS_PER_MENU_ITEM
  #define STD_ENCODER_STEPS_PER_MENU_ITEM 1
#endif
#ifndef ENCODER_PULSES_PER_STEP
  #define ENCODER_PULSES_PER_STEP STD_ENCODER_PULSES_PER_STEP
#endif
#ifndef ENCODER_STEPS_PER_MENU_ITEM
  #define ENCODER_STEPS_PER_MENU_ITEM STD_ENCODER_STEPS_PER_MENU_ITEM
#endif
#ifndef ENCODER_FEEDRATE_DEADZONE
  #define ENCODER_FEEDRATE_DEADZONE 6
#endif

// Shift register panels
// ---------------------
// 2 wire Non-latching LCD SR from:
// https://bitbucket.org/fmalpartida/new-liquidcrystal/wiki/schematics#!shiftregister-connection
#if ENABLED(FF_INTERFACEBOARD)
  #define SR_LCD_3W_NL    // Non latching 3 wire shift register
  #define IS_ULTIPANEL 1
#elif ENABLED(SAV_3DLCD)
  #define SR_LCD_2W_NL    // Non latching 2 wire shift register
  #define IS_ULTIPANEL 1
#elif ENABLED(ULTIPANEL)
  #define IS_ULTIPANEL 1
#endif

#if EITHER(IS_ULTIPANEL, ULTRA_LCD)
  #define IS_ULTRA_LCD 1
#endif

#if EITHER(IS_ULTIPANEL, REPRAPWORLD_KEYPAD)
  #define IS_NEWPANEL 1
#endif

#if EITHER(ZONESTAR_LCD, REPRAPWORLD_KEYPAD)
  #define IS_RRW_KEYPAD 1
  #ifndef REPRAPWORLD_KEYPAD_MOVE_STEP
    #define REPRAPWORLD_KEYPAD_MOVE_STEP 1.0
  #endif
#endif

// Aliases for LCD features
#if ANY(DGUS_LCD_UI_ORIGIN, DGUS_LCD_UI_FYSETC, DGUS_LCD_UI_HIPRECY, DGUS_LCD_UI_MKS)
  #define HAS_DGUS_LCD_CLASSIC 1
#endif

#if ANY(HAS_DGUS_LCD_CLASSIC, DGUS_LCD_UI_RELOADED)
  #define HAS_DGUS_LCD 1
#endif

// Extensible UI serial touch screens. (See src/lcd/extui)
#if ANY(HAS_DGUS_LCD, MALYAN_LCD, TOUCH_UI_FTDI_EVE, ANYCUBIC_LCD_I3MEGA, ANYCUBIC_LCD_CHIRON, NEXTION_TFT)
  #define IS_EXTUI 1
  #define EXTENSIBLE_UI
#endif

// Aliases for LCD features
#if EITHER(DWIN_CREALITY_LCD, DWIN_CREALITY_LCD_ENHANCED)
  #define HAS_DWIN_E3V2_BASIC 1
#endif
#if EITHER(HAS_DWIN_E3V2_BASIC, DWIN_CREALITY_LCD_JYERSUI)
  #define HAS_DWIN_E3V2 1
#endif

// E3V2 extras
#if HAS_DWIN_E3V2 || IS_DWIN_MVCNCUI
  #define SERIAL_CATCHALL 0
  #ifndef LCD_SERIAL_PORT
    #if MB(BTT_SKR_MINI_E3_V1_0, BTT_SKR_MINI_E3_V1_2, BTT_SKR_MINI_E3_V2_0, BTT_SKR_MINI_E3_V3_0, BTT_SKR_E3_TURBO)
      #define LCD_SERIAL_PORT 1
    #elif MB(CREALITY_V24S1_301)
      #define LCD_SERIAL_PORT 2 // Creality Ender3S1 board
    #else
      #define LCD_SERIAL_PORT 3 // Creality 4.x board
    #endif
  #endif
  #define HAS_LCD_BRIGHTNESS 1
  #define LCD_BRIGHTNESS_MAX 250
  #if ENABLED(DWIN_CREALITY_LCD_ENHANCED)
    #define LCD_BRIGHTNESS_DEFAULT 127
  #endif
#endif

#if IS_ULTRA_LCD
  #define HAS_WIRED_LCD 1
  #if ENABLED(DOGLCD)
    #define HAS_MVCNCUI_U8GLIB 1
  #elif IS_TFTGLCD_PANEL
    // Neither DOGM nor HD44780. Fully customized interface.
  #elif IS_DWIN_MVCNCUI
    // Since HAS_MVCNCUI_U8GLIB refers to U8G displays
    // the DWIN display can define its own flags
  #elif !HAS_GRAPHICAL_TFT
    #define HAS_MVCNCUI_HD44780 1
  #endif
#endif

#if ANY(HAS_WIRED_LCD, EXTENSIBLE_UI, DWIN_CREALITY_LCD_JYERSUI)
  #define HAS_DISPLAY 1
#endif

#if ANY(HAS_DISPLAY, HAS_DWIN_E3V2, GLOBAL_STATUS_MESSAGE)
  #define HAS_STATUS_MESSAGE 1
#endif

#if IS_ULTIPANEL && DISABLED(NO_LCD_MENUS)
  #define HAS_MVCNCUI_MENU 1
#endif

#if ANY(HAS_MVCNCUI_MENU, EXTENSIBLE_UI, HAS_DWIN_E3V2)
  #define HAS_MANUAL_MOVE_MENU 1
#endif

#if ANY(HAS_MVCNCUI_U8GLIB, EXTENSIBLE_UI, HAS_MVCNCUI_HD44780, IS_TFTGLCD_PANEL, IS_DWIN_MVCNCUI, DWIN_CREALITY_LCD_JYERSUI)
  #define CAN_SHOW_REMAINING_TIME 1
#endif

#if HAS_MVCNCUI_U8GLIB
  #ifndef LCD_PIXEL_WIDTH
    #define LCD_PIXEL_WIDTH 128
  #endif
  #ifndef LCD_PIXEL_HEIGHT
    #define LCD_PIXEL_HEIGHT 64
  #endif
#endif

  #undef ATC_TOOLS
  #define ATC_TOOLS 0
  #undef SINGLENOZZLE
  #undef SWITCHING_EXTRUDER
  #undef SWITCHING_NOZZLE
  #undef MIXING_EXTRUDER
  #undef HOTEND_IDLE_TIMEOUT
  #undef DISABLE_E

#define E_OPTARG(N) OPTARG(TOOL_CHANGE_SUPPORT, N)
#define E_TERN_(N)  TERN_(TOOL_CHANGE_SUPPORT, N)
#define E_TERN0(N)  TERN0(TOOL_CHANGE_SUPPORT, N)
#define HOTENDS ATC_TOOLS
#define E_STEPPERS ATC_TOOLS
#define E_MANUAL ATC_TOOLS

/**
 * Number of Linear Axes (e.g., XYZ)
 * All the logical axes except for the tool (E) axis
 */
#ifndef LINEAR_AXES
  #define LINEAR_AXES XYZ
#endif
#if LINEAR_AXES >= XY
  #define HAS_Y_AXIS 1
  #if LINEAR_AXES >= XYZ
    #define HAS_Z_AXIS 1
    #if LINEAR_AXES >= 4
      #define HAS_I_AXIS 1
      #if LINEAR_AXES >= 5
        #define HAS_J_AXIS 1
        #if LINEAR_AXES >= 6
          #define HAS_K_AXIS 1
        #endif
      #endif
    #endif
  #endif
#endif

/**
 * Number of Logical Axes (e.g., XYZE)
 * All the logical axes that can be commanded directly by G-code.
 * Delta maps stepper-specific values to ABC steppers.
 */
  #define LOGICAL_AXES LINEAR_AXES

  #undef DISTINCT_E_FACTORS
  #define DISTINCT_AXES LOGICAL_AXES
  #define DISTINCT_E 1
  #define E_INDEX_N(E) 0

  #undef PID_PARAMS_PER_HOTEND

// Helper macros for ATC tool and hotend arrays
#define HOTEND_LOOP() for (int8_t e = 0; e < HOTENDS; e++)
#define ARRAY_BY_ATC_TOOLS(V...) ARRAY_N(ATC_TOOLS, V)
#define ARRAY_BY_ATC_TOOLS1(v1) ARRAY_N_1(ATC_TOOLS, v1)

/**
 * Disable unused SINGLENOZZLE sub-options
 */
#if DISABLED(SINGLENOZZLE)
  #undef SINGLENOZZLE_STANDBY_TEMP
#endif
#if !BOTH(HAS_FAN, SINGLENOZZLE)
  #undef SINGLENOZZLE_STANDBY_FAN
#endif

/**
 * Set a flag for a servo probe (or BLTouch)
 */
#ifdef Z_PROBE_SERVO_NR
  #define HAS_Z_SERVO_PROBE 1
#endif
#if ANY(HAS_Z_SERVO_PROBE, SWITCHING_EXTRUDER, SWITCHING_NOZZLE)
  #define HAS_SERVO_ANGLES 1
#endif
#if !HAS_SERVO_ANGLES
  #undef EDITABLE_SERVO_ANGLES
#endif

/**
 * Set a flag for any type of bed probe, including the paper-test
 */
#if ANY(HAS_Z_SERVO_PROBE, FIX_MOUNTED_PROBE, NOZZLE_AS_PROBE, TOUCH_MI_PROBE, Z_PROBE_ALLEN_KEY, Z_PROBE_SLED, SOLENOID_PROBE, SENSORLESS_PROBING, RACK_AND_PINION_PROBE, MAGLEV4)
  #define HAS_BED_PROBE 1
#endif

// Homing to Min or Max
#if X_HOME_DIR > 0
  #define X_HOME_TO_MAX 1
#elif X_HOME_DIR < 0
  #define X_HOME_TO_MIN 1
#endif
#if Y_HOME_DIR > 0
  #define Y_HOME_TO_MAX 1
#elif Y_HOME_DIR < 0
  #define Y_HOME_TO_MIN 1
#endif
#if Z_HOME_DIR > 0
  #define Z_HOME_TO_MAX 1
#elif Z_HOME_DIR < 0
  #define Z_HOME_TO_MIN 1
#endif
#if I_HOME_DIR > 0
  #define I_HOME_TO_MAX 1
#elif I_HOME_DIR < 0
  #define I_HOME_TO_MIN 1
#endif
#if J_HOME_DIR > 0
  #define J_HOME_TO_MAX 1
#elif J_HOME_DIR < 0
  #define J_HOME_TO_MIN 1
#endif
#if K_HOME_DIR > 0
  #define K_HOME_TO_MAX 1
#elif K_HOME_DIR < 0
  #define K_HOME_TO_MIN 1
#endif

/**
 * Conditionals based on the type of Bed Probe
 */
#if HAS_BED_PROBE
  #if DISABLED(NOZZLE_AS_PROBE)
    #define HAS_PROBE_XY_OFFSET 1
  #endif
  #if BOTH(DELTA, SENSORLESS_PROBING)
    #define HAS_DELTA_SENSORLESS_PROBING 1
  #endif
  #if NONE(Z_MIN_PROBE_USES_Z_MIN_ENDSTOP_PIN, HAS_DELTA_SENSORLESS_PROBING)
    #define USES_Z_MIN_PROBE_PIN 1
  #endif
  #if Z_HOME_TO_MIN && TERN1(USES_Z_MIN_PROBE_PIN, ENABLED(USE_PROBE_FOR_Z_HOMING))
    #define HOMING_Z_WITH_PROBE 1
  #endif
  #ifndef Z_PROBE_LOW_POINT
    #define Z_PROBE_LOW_POINT -5
  #endif
  #if ENABLED(Z_PROBE_ALLEN_KEY)
    #define PROBE_TRIGGERED_WHEN_STOWED_TEST 1 // Extra test for Allen Key Probe
  #endif
  #if MULTIPLE_PROBING > 1
    #if EXTRA_PROBING > 0
      #define TOTAL_PROBING (MULTIPLE_PROBING + EXTRA_PROBING)
    #else
      #define TOTAL_PROBING MULTIPLE_PROBING
    #endif
  #endif
#else
  // Clear probe pin settings when no probe is selected
  #undef Z_MIN_PROBE_USES_Z_MIN_ENDSTOP_PIN
  #undef USE_PROBE_FOR_Z_HOMING
#endif

#if Z_HOME_TO_MAX
  #define HOME_Z_FIRST // If homing away from BED do Z first
#endif

  #undef RESTORE_LEVELING_AFTER_G28
  #undef ENABLE_LEVELING_AFTER_G28
  #undef G29_RETRY_AND_RECOVER
  #undef PROBE_MANUALLY
#if ANY(HAS_BED_PROBE, PROBE_MANUALLY)
  #define PROBE_SELECTED 1
#endif

// Slim menu optimizations
#if ENABLED(SLIM_LCD_MENUS)
  #define BOOT_MVCNC_LOGO_SMALL
#endif

/**
 * CoreXY, CoreXZ, and CoreYZ - and their reverse
 */
#if EITHER(COREXY, COREYX)
  #define CORE_IS_XY 1
#endif
#if EITHER(COREXZ, COREZX)
  #define CORE_IS_XZ 1
#endif
#if EITHER(COREYZ, COREZY)
  #define CORE_IS_YZ 1
#endif
#if CORE_IS_XY || CORE_IS_XZ || CORE_IS_YZ
  #define IS_CORE 1
#endif
#if IS_CORE
  #if CORE_IS_XY
    #define CORE_AXIS_1 A_AXIS
    #define CORE_AXIS_2 B_AXIS
    #define NORMAL_AXIS Z_AXIS
  #elif CORE_IS_XZ
    #define CORE_AXIS_1 A_AXIS
    #define NORMAL_AXIS Y_AXIS
    #define CORE_AXIS_2 C_AXIS
  #elif CORE_IS_YZ
    #define NORMAL_AXIS X_AXIS
    #define CORE_AXIS_1 B_AXIS
    #define CORE_AXIS_2 C_AXIS
  #endif
  #define CORESIGN(n) (ANY(COREYX, COREZX, COREZY) ? (-(n)) : (n))
#endif

#if ENABLED(DELTA)
  #define IS_KINEMATIC 1
#else
  #define IS_CARTESIAN 1
  #if !IS_CORE
    #define IS_FULL_CARTESIAN 1
  #endif
  #undef DELTA_HOME_TO_SAFE_ZONE
#endif

// This flag indicates some kind of jerk storage is needed
#if EITHER(CLASSIC_JERK, IS_KINEMATIC)
  #define HAS_CLASSIC_JERK 1
#endif

#if DISABLED(CLASSIC_JERK)
  #define HAS_JUNCTION_DEVIATION 1
#endif

//
// Serial Port Info
//
#ifdef SERIAL_PORT_2
  #define HAS_MULTI_SERIAL 1
  #ifdef SERIAL_PORT_3
    #define NUM_SERIAL 3
  #else
    #define NUM_SERIAL 2
  #endif
#elif defined(SERIAL_PORT)
  #define NUM_SERIAL 1
#else
  #define NUM_SERIAL 0
  #undef BAUD_RATE_GCODE
#endif
#if SERIAL_PORT == -1 || SERIAL_PORT_2 == -1 || SERIAL_PORT_3 == -1
  #define HAS_USB_SERIAL 1
#endif
#if SERIAL_PORT_2 == -2
  #define HAS_ETHERNET 1
#endif

// Fallback Stepper Driver types that don't depend on Configuration_adv.h
#ifndef X_DRIVER_TYPE
  #define X_DRIVER_TYPE  A4988
#endif
#ifndef X2_DRIVER_TYPE
  #define X2_DRIVER_TYPE A4988
#endif
#ifndef Y_DRIVER_TYPE
  #define Y_DRIVER_TYPE  A4988
#endif
#ifndef Y2_DRIVER_TYPE
  #define Y2_DRIVER_TYPE A4988
#endif
#ifndef Z_DRIVER_TYPE
  #define Z_DRIVER_TYPE  A4988
#endif
#ifndef Z2_DRIVER_TYPE
  #define Z2_DRIVER_TYPE A4988
#endif
#ifndef Z3_DRIVER_TYPE
  #define Z3_DRIVER_TYPE A4988
#endif
#ifndef Z4_DRIVER_TYPE
  #define Z4_DRIVER_TYPE A4988
#endif
#undef E0_DRIVER_TYPE
#undef E1_DRIVER_TYPE
#undef E2_DRIVER_TYPE
#undef E3_DRIVER_TYPE
#undef E4_DRIVER_TYPE
#undef E5_DRIVER_TYPE
#undef E6_DRIVER_TYPE
#undef E7_DRIVER_TYPE

// Fallback axis inverting
#ifndef INVERT_X_DIR
  #define INVERT_X_DIR false
#endif
#if HAS_Y_AXIS && !defined(INVERT_Y_DIR)
  #define INVERT_Y_DIR false
#endif
#if HAS_Z_AXIS && !defined(INVERT_Z_DIR)
  #define INVERT_Z_DIR false
#endif
#if HAS_I_AXIS && !defined(INVERT_I_DIR)
  #define INVERT_I_DIR false
#endif
#if HAS_J_AXIS && !defined(INVERT_J_DIR)
  #define INVERT_J_DIR false
#endif
#if HAS_K_AXIS && !defined(INVERT_K_DIR)
  #define INVERT_K_DIR false
#endif

/**
 * TFT Displays
 *
 * Configure parameters for TFT displays:
 *  - TFT_DEFAULT_ORIENTATION
 *  - TFT_DRIVER
 *  - TFT_WIDTH
 *  - TFT_HEIGHT
 *  - TFT_INTERFACE_(SPI|FSMC)
 *  - TFT_COLOR
 *  - GRAPHICAL_TFT_UPSCALE
 */
#if EITHER(MKS_TS35_V2_0, BTT_TFT35_SPI_V1_0)                                 // ST7796
  #define TFT_DEFAULT_DRIVER ST7796
  #define TFT_DEFAULT_ORIENTATION TFT_EXCHANGE_XY
  #define TFT_RES_480x320
  #define TFT_INTERFACE_SPI
#elif EITHER(LERDGE_TFT35, ANET_ET5_TFT35)                                    // ST7796
  #define TFT_DEFAULT_ORIENTATION TFT_EXCHANGE_XY
  #define TFT_RES_480x320
  #define TFT_INTERFACE_FSMC
#elif ANY(ANET_ET4_TFT28, MKS_ROBIN_TFT24, MKS_ROBIN_TFT28, MKS_ROBIN_TFT32)  // ST7789
  #define TFT_DEFAULT_ORIENTATION (TFT_EXCHANGE_XY | TFT_INVERT_Y)
  #define TFT_RES_320x240
  #define TFT_INTERFACE_FSMC
#elif ANY(MKS_ROBIN_TFT35, TFT_TRONXY_X5SA, ANYCUBIC_TFT35)                   // ILI9488
  #define TFT_DRIVER ILI9488
  #define TFT_DEFAULT_ORIENTATION (TFT_EXCHANGE_XY | TFT_INVERT_X | TFT_INVERT_Y)
  #define TFT_RES_480x320
  #define TFT_INTERFACE_FSMC
#elif ENABLED(MKS_ROBIN_TFT43)
  #define TFT_DRIVER SSD1963
  #define TFT_DEFAULT_ORIENTATION 0
  #define TFT_RES_480x272
  #define TFT_INTERFACE_FSMC
#elif ANY(MKS_ROBIN_TFT_V1_1R, LONGER_LK_TFT28)                               // ILI9328 or R61505
  #define TFT_DEFAULT_ORIENTATION (TFT_EXCHANGE_XY | TFT_INVERT_X | TFT_INVERT_Y)
  #define TFT_RES_320x240
  #define TFT_INTERFACE_FSMC
#elif ENABLED(BIQU_BX_TFT70)                                                  // RGB
  #define TFT_DEFAULT_ORIENTATION TFT_EXCHANGE_XY
  #define TFT_RES_1024x600
  #define TFT_INTERFACE_LTDC
  #if ENABLED(TOUCH_SCREEN)
    #define TFT_TOUCH_DEVICE_GT911
  #endif
#elif ENABLED(TFT_GENERIC)
  #define TFT_DEFAULT_ORIENTATION (TFT_EXCHANGE_XY | TFT_INVERT_X | TFT_INVERT_Y)
  #if NONE(TFT_RES_320x240, TFT_RES_480x272, TFT_RES_480x320)
    #define TFT_RES_320x240
  #endif
  #if NONE(TFT_INTERFACE_FSMC, TFT_INTERFACE_SPI)
    #define TFT_INTERFACE_SPI
  #endif
#endif

#if ENABLED(TFT_RES_320x240)
  #define TFT_WIDTH  320
  #define TFT_HEIGHT 240
  #define GRAPHICAL_TFT_UPSCALE 2
#elif ENABLED(TFT_RES_480x272)
  #define TFT_WIDTH  480
  #define TFT_HEIGHT 272
  #define GRAPHICAL_TFT_UPSCALE 2
#elif ENABLED(TFT_RES_480x320)
  #define TFT_WIDTH  480
  #define TFT_HEIGHT 320
  #define GRAPHICAL_TFT_UPSCALE 3
#elif ENABLED(TFT_RES_1024x600)
  #define TFT_WIDTH  1024
  #define TFT_HEIGHT 600
  #define GRAPHICAL_TFT_UPSCALE 4
#endif

// FSMC/SPI TFT Panels using standard HAL/tft/tft_(fsmc|spi|ltdc).h
#if ENABLED(TFT_INTERFACE_FSMC)
  #define HAS_FSMC_TFT 1
  #if TFT_SCALED_DOGLCD
    #define HAS_FSMC_GRAPHICAL_TFT 1
  #elif HAS_TFT_LVGL_UI
    #define HAS_TFT_LVGL_UI_FSMC 1
  #endif
#elif ENABLED(TFT_INTERFACE_SPI)
  #define HAS_SPI_TFT 1
  #if TFT_SCALED_DOGLCD
    #define HAS_SPI_GRAPHICAL_TFT 1
  #elif HAS_TFT_LVGL_UI
    #define HAS_TFT_LVGL_UI_SPI 1
  #endif
#elif ENABLED(TFT_INTERFACE_LTDC)
  #define HAS_LTDC_TFT 1
  #if TFT_SCALED_DOGLCD
    #define HAS_LTDC_GRAPHICAL_TFT 1
  #elif HAS_TFT_LVGL_UI
    #define HAS_TFT_LVGL_UI_LTDC 1
  #endif
#endif

#if ENABLED(TFT_COLOR_UI)
  #if TFT_HEIGHT == 240
    #if ENABLED(TFT_INTERFACE_SPI)
      #define TFT_320x240_SPI
    #elif ENABLED(TFT_INTERFACE_FSMC)
      #define TFT_320x240
    #endif
  #elif TFT_HEIGHT == 320
    #if ENABLED(TFT_INTERFACE_SPI)
      #define TFT_480x320_SPI
    #elif ENABLED(TFT_INTERFACE_FSMC)
      #define TFT_480x320
    #endif
  #elif TFT_HEIGHT == 272
    #if ENABLED(TFT_INTERFACE_SPI)
      #define TFT_480x272_SPI
    #elif ENABLED(TFT_INTERFACE_FSMC)
      #define TFT_480x272
    #endif
  #elif TFT_HEIGHT == 600
    #if ENABLED(TFT_INTERFACE_LTDC)
      #define TFT_1024x600_LTDC
    #endif
  #endif
#endif

#if EITHER(TFT_320x240, TFT_320x240_SPI)
  #define HAS_UI_320x240 1
#elif EITHER(TFT_480x320, TFT_480x320_SPI)
  #define HAS_UI_480x320 1
#elif EITHER(TFT_480x272, TFT_480x272_SPI)
  #define HAS_UI_480x272 1
#elif defined(TFT_1024x600_LTDC)
  #define HAS_UI_1024x600 1
#endif
#if ANY(HAS_UI_320x240, HAS_UI_480x320, HAS_UI_480x272)
  #define LCD_HEIGHT TERN(TOUCH_SCREEN, 6, 7)   // Fewer lines with touch buttons onscreen
#elif HAS_UI_1024x600
  #define LCD_HEIGHT TERN(TOUCH_SCREEN, 12, 13) // Fewer lines with touch buttons onscreen
#endif

// This emulated DOGM has 'touch/xpt2046', not 'tft/xpt2046'
#if ENABLED(TOUCH_SCREEN)
  #if TOUCH_IDLE_SLEEP
    #define HAS_TOUCH_SLEEP 1
  #endif
  #if NONE(TFT_TOUCH_DEVICE_GT911, TFT_TOUCH_DEVICE_XPT2046)
    #define TFT_TOUCH_DEVICE_XPT2046          // ADS7843/XPT2046 ADC Touchscreen such as ILI9341 2.8
  #endif
  #if ENABLED(TFT_TOUCH_DEVICE_GT911)         // GT911 Capacitive touch screen such as BIQU_BX_TFT70
    #undef TOUCH_SCREEN_CALIBRATION
    #undef TOUCH_CALIBRATION_AUTO_SAVE
  #endif
  #if !HAS_GRAPHICAL_TFT
    #undef TOUCH_SCREEN
    #if ENABLED(TFT_CLASSIC_UI)
      #define HAS_TOUCH_BUTTONS 1
      #if ENABLED(TFT_TOUCH_DEVICE_GT911)
        #define HAS_CAP_TOUCH_BUTTONS 1
      #else
        #define HAS_RES_TOUCH_BUTTONS 1
      #endif
    #endif
  #endif
#endif

// XPT2046_** Compatibility
#if !(defined(TOUCH_CALIBRATION_X) || defined(TOUCH_CALIBRATION_Y) || defined(TOUCH_OFFSET_X) || defined(TOUCH_OFFSET_Y) || defined(TOUCH_ORIENTATION))
  #if defined(XPT2046_X_CALIBRATION) && defined(XPT2046_Y_CALIBRATION) && defined(XPT2046_X_OFFSET) && defined(XPT2046_Y_OFFSET)
    #define TOUCH_CALIBRATION_X  XPT2046_X_CALIBRATION
    #define TOUCH_CALIBRATION_Y  XPT2046_Y_CALIBRATION
    #define TOUCH_OFFSET_X       XPT2046_X_OFFSET
    #define TOUCH_OFFSET_Y       XPT2046_Y_OFFSET
    #define TOUCH_ORIENTATION    TOUCH_LANDSCAPE
  #endif
#endif

#if X_HOME_DIR || (HAS_Y_AXIS && Y_HOME_DIR) || (HAS_Z_AXIS && Z_HOME_DIR) || (HAS_I_AXIS && I_HOME_DIR) || (HAS_J_AXIS && J_HOME_DIR) || (HAS_K_AXIS && K_HOME_DIR)
  #define HAS_ENDSTOPS 1
  #define COORDINATE_OKAY(N,L,H) WITHIN(N,L,H)
#else
  #define COORDINATE_OKAY(N,L,H) true
#endif

/**
 * LED Backlight INDEX END
 */
#if defined(NEOPIXEL_BKGD_INDEX_FIRST) && !defined(NEOPIXEL_BKGD_INDEX_LAST)
  #define NEOPIXEL_BKGD_INDEX_LAST NEOPIXEL_BKGD_INDEX_FIRST
#endif
