/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
/********************************************************************************
 * @file     lcd/e3v2/mvcncui/dwin_lcd.cpp
 * @brief    DWIN screen control functions
 ********************************************************************************/

#include "../../../inc/mvCNCConfigPre.h"

#if IS_DWIN_MVCNCUI

#include "../../../inc/mvCNCConfig.h"

#include "dwin_lcd.h"
#include <string.h> // for memset

//#define DEBUG_OUT 1
#include "../../../core/debug_out.h"

/*-------------------------------------- System variable function --------------------------------------*/

void DWIN_Startup() {
  DEBUG_ECHOPGM("\r\nDWIN handshake ");
  delay(750);   // Delay here or init later in the boot process
  const bool success = DWIN_Handshake();
  if (success) DEBUG_ECHOLNPGM("ok."); else DEBUG_ECHOLNPGM("error.");
  DWIN_Frame_SetDir(TERN(DWIN_mvCNCUI_LANDSCAPE, 0, 1));
  DWIN_Frame_Clear(Color_Bg_Black); // mvCNCUI handles the bootscreen so just clear here
  DWIN_UpdateLCD();
}

/*---------------------------------------- Picture related functions ----------------------------------------*/

// Draw an Icon
//  libID: Icon library ID
//  picID: Icon ID
//  x/y: Upper-left point
void DWIN_ICON_Show(uint8_t libID, uint8_t picID, uint16_t x, uint16_t y) {
  DWIN_ICON_Show(true, false, false, libID, picID, x, y);
}

#endif // IS_DWIN_MVCNCUI
