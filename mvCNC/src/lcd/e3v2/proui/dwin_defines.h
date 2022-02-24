/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

/**
 * DWIN general defines and data structs
 * Author: Miguel A. Risco-Castillo (MRISCOC)
 * Version: 3.9.2
 * Date: 2021/11/21
 *
 * Based on the original code provided by Creality under GPL
 */

//#define NEED_HEX_PRINT 1
//#define DEBUG_DWIN 1

#include "../../../core/types.h"
#include "../common/dwin_color.h"
#if ENABLED(LED_CONTROL_MENU)
  #include "../../../feature/leds/leds.h"
#endif

#define Def_Background_Color  RGB( 1, 12,  8)
#define Def_Cursor_color      RGB(20, 49, 31)
#define Def_TitleBg_color     RGB( 0, 23, 16)
#define Def_TitleTxt_color    Color_White
#define Def_Text_Color        Color_White
#define Def_Selected_Color    Select_Color
#define Def_SplitLine_Color   RGB( 0, 23, 16)
#define Def_Highlight_Color   Color_White
#define Def_StatusBg_Color    RGB( 0, 23, 16)
#define Def_StatusTxt_Color   Color_Yellow
#define Def_PopupBg_color     Color_Bg_Window
#define Def_PopupTxt_Color    Popup_Text_Color
#define Def_AlertBg_Color     Color_Bg_Red
#define Def_AlertTxt_Color    Color_Yellow
#define Def_PercentTxt_Color  Percent_Color
#define Def_Barfill_Color     BarFill_Color
#define Def_Indicator_Color   Color_White
#define Def_Coordinate_Color  Color_White

//#define HAS_GCODE_PREVIEW 1
#define HAS_ESDIAG 1

#if ENABLED(LED_CONTROL_MENU, HAS_COLOR_LEDS)
  #define Def_Leds_Color      LEDColorWhite()
#endif
#if ENABLED(CASELIGHT_USES_BRIGHTNESS)
  #define Def_CaseLight_Brightness 255
#endif

typedef struct {
  // Color settings
  uint16_t Background_Color = Def_Background_Color;
  uint16_t Cursor_color     = Def_Cursor_color;
  uint16_t TitleBg_color    = Def_TitleBg_color;
  uint16_t TitleTxt_color   = Def_TitleTxt_color;
  uint16_t Text_Color       = Def_Text_Color;
  uint16_t Selected_Color   = Def_Selected_Color;
  uint16_t SplitLine_Color  = Def_SplitLine_Color;
  uint16_t Highlight_Color  = Def_Highlight_Color;
  uint16_t StatusBg_Color   = Def_StatusBg_Color;
  uint16_t StatusTxt_Color  = Def_StatusTxt_Color;
  uint16_t PopupBg_color    = Def_PopupBg_color;
  uint16_t PopupTxt_Color   = Def_PopupTxt_Color;
  uint16_t AlertBg_Color    = Def_AlertBg_Color;
  uint16_t AlertTxt_Color   = Def_AlertTxt_Color;
  uint16_t PercentTxt_Color = Def_PercentTxt_Color;
  uint16_t Barfill_Color    = Def_Barfill_Color;
  uint16_t Indicator_Color  = Def_Indicator_Color;
  uint16_t Coordinate_Color = Def_Coordinate_Color;
  // Temperatures
  #if HAS_HOTEND && defined(PREHEAT_1_TEMP_HOTEND)
    int16_t HotendPidT = PREHEAT_1_TEMP_HOTEND;
  #endif
  #if HAS_HEATED_BED && defined(PREHEAT_1_TEMP_BED)
    int16_t BedPidT = PREHEAT_1_TEMP_BED;
  #endif
  #if HAS_HOTEND || HAS_HEATED_BED
    int16_t PidCycles = 10;
  #endif
  #if ENABLED(PREVENT_COLD_EXTRUSION)
    int16_t ExtMinT = EXTRUDE_MINTEMP;
  #endif
  // Led
  #if BOTH(LED_CONTROL_MENU, HAS_COLOR_LEDS)
    LEDColor Led_Color = Def_Leds_Color;
  #endif
  // Case Light
  #if ENABLED(CASELIGHT_USES_BRIGHTNESS)
    uint8_t CaseLight_Brightness = Def_CaseLight_Brightness;
  #endif
} HMI_data_t;

static constexpr size_t eeprom_data_size = 64;
extern HMI_data_t HMI_data;
