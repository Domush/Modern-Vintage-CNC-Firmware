/**
 * Modern Vintage CNC Firmware
*/
#pragma once

#include "../../inc/mvCNCConfigPre.h"

#if ENABLED(NO_LCD_MENUS)
  #error "Seriously? High resolution TFT screen without menu?"
#endif

#include "tft.h"
#include "tft_image.h"

#if ENABLED(TOUCH_SCREEN)
  #include "touch.h"
  extern bool draw_menu_navigation;
#endif

#if HAS_UI_320x240
  #include "ui_320x240.h"
#elif HAS_UI_480x320 || HAS_UI_480x272
  #include "ui_480x320.h"
#elif HAS_UI_1024x600
  #include "ui_1024x600.h"
#else
  #error "Unsupported display resolution!"
#endif

void draw_heater_status(uint16_t x, uint16_t y, const int8_t Heater);
void draw_fan_status(uint16_t x, uint16_t y, const bool blink);

void menu_line(const uint8_t row, uint16_t color=COLOR_BACKGROUND);
void menu_item(const uint8_t row, bool sel = false);

#if HAS_TOUCH_SLEEP
  bool lcd_sleep_task();
#endif

#define ABSOLUTE_ZERO     -273.15

#if HAS_TEMP_CHAMBER && TOOL_CHANGE_SUPPORT
  #define ITEM_E0         0
  #define ITEM_E1         1
  #define ITEM_BED        2
  #define ITEM_CHAMBER    3
  #define ITEM_FAN        4
  #define ITEMS_COUNT     5
#elif HAS_TEMP_CHAMBER
  #define ITEM_E0         0
  #define ITEM_BED        1
  #define ITEM_CHAMBER    2
  #define ITEM_FAN        3
  #define ITEMS_COUNT     4
#elif HAS_TEMP_COOLER
  #define ITEM_COOLER     0
  #define ITEM_FAN        1
  #define ITEMS_COUNT     2
#elif TOOL_CHANGE_SUPPORT
  #define ITEM_E0         0
  #define ITEM_E1         1
  #define ITEM_BED        2
  #define ITEM_FAN        3
  #define ITEMS_COUNT     4
#else
  #define ITEM_E0         0
  #define ITEM_BED        1
  #define ITEM_FAN        2
  #define ITEMS_COUNT     3
#endif
