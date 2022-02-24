/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
/**
 * Lock screen implementation for DWIN UI Enhanced implementation
 * Author: Miguel A. Risco-Castillo (MRISCOC)
 * Version: 2.1
 * Date: 2021/11/09
 *
 * Based on the original code provided by Creality under GPL
 */

#include "../../../inc/mvCNCConfigPre.h"

#if ENABLED(DWIN_CREALITY_LCD_ENHANCED)

#include "../../../core/types.h"
#include "dwin_lcd.h"
#include "dwinui.h"
#include "dwin.h"
#include "lockscreen.h"

LockScreenClass lockScreen;

uint8_t LockScreenClass::lock_pos = 0;
bool LockScreenClass::unlocked = false;
uint8_t LockScreenClass::rprocess = 0;

void LockScreenClass::init() {
  lock_pos = 0;
  unlocked = false;
  draw();
}

void LockScreenClass::draw() {
  Title.SetCaption(PSTR("Lock Screen"));
  DWINUI::ClearMenuArea();
  DWINUI::Draw_Icon(ICON_LOGO, 71, 120);  // CREALITY logo
  DWINUI::Draw_CenteredString(Color_White, 180, F("CNC is Locked,"));
  DWINUI::Draw_CenteredString(Color_White, 200, F("Scroll to unlock."));
  DWINUI::Draw_CenteredString(Color_White, 240, F("-> | <-"));
  DWIN_Draw_Box(1, HMI_data.Barfill_Color, 0, 260, DWIN_WIDTH, 20);
  DWIN_Draw_VLine(Color_Yellow, lock_pos * DWIN_WIDTH / 255, 260, 20);
  DWIN_UpdateLCD();
}

void LockScreenClass::onEncoder(EncoderState encoder_diffState) {
  switch (encoder_diffState) {
    case ENCODER_DIFF_CW:    lock_pos += 8; break;
    case ENCODER_DIFF_CCW:   lock_pos -= 8; break;
    case ENCODER_DIFF_ENTER: unlocked = (lock_pos == 128); break;
    default: break;
  }
  DWIN_Draw_Box(1, HMI_data.Barfill_Color, 0, 260, DWIN_WIDTH, 20);
  DWIN_Draw_VLine(Color_Yellow, lock_pos * DWIN_WIDTH / 255, 260, 20);
  DWIN_UpdateLCD();
}

#endif // DWIN_CREALITY_LCD_ENHANCED
