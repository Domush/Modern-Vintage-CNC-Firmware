/**
 * Modern Vintage CNC Firmware
*/
#pragma once

#define MVCNC_LOGO_FULL_SIZE mvCNCLogo480x320x16

#include "ui_common.h"

#define TFT_STATUS_TOP_Y       4
#define TFT_TOP_LINE_Y         4

#define MENU_TEXT_X_OFFSET    16
#define MENU_TEXT_Y_OFFSET     7

#define MENU_ITEM_ICON_X       5
#define MENU_ITEM_ICON_Y       5
#define MENU_ITEM_ICON_SPACE  42

#if HAS_UI_480x320
  #define MENU_FONT_NAME      Helvetica18
  #define SYMBOLS_FONT_NAME   Helvetica18_symbols
  #define MENU_ITEM_HEIGHT    43
  #define FONT_LINE_HEIGHT    34
#elif HAS_UI_480x272
  #define MENU_FONT_NAME      Helvetica14
  #define SYMBOLS_FONT_NAME   Helvetica14_symbols
  #define MENU_ITEM_HEIGHT    36
  #define FONT_LINE_HEIGHT    24
#endif
#define MENU_LINE_HEIGHT      (MENU_ITEM_HEIGHT + 2)
