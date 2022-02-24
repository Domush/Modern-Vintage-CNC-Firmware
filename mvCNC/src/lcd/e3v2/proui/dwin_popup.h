/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

/**
 * DWIN UI Enhanced implementation
 * Author: Miguel A. Risco-Castillo (MRISCOC)
 * Version: 3.10.1
 * Date: 2022/01/21
 *
 * Based on the original code provided by Creality under GPL
 */

#include "dwinui.h"
#include "dwin.h"

// Popup windows

void Draw_Select_Highlight(const bool sel);

inline void Draw_Popup_Bkgd() {
  DWIN_Draw_Rectangle(1, HMI_data.PopupBg_color, 14, 60, 258, 330);
  DWIN_Draw_Rectangle(0, HMI_data.Highlight_Color, 14, 60, 258, 330);
}

template<typename T, typename U>
void DWIN_Draw_Popup(const uint8_t icon, T amsg1=nullptr, U amsg2=nullptr, uint8_t button=0) {
  DWINUI::ClearMenuArea();
  Draw_Popup_Bkgd();
  if (icon) DWINUI::Draw_Icon(icon, 101, 105);
  if (amsg1) DWINUI::Draw_CenteredString(HMI_data.PopupTxt_Color, 210, amsg1);
  if (amsg2) DWINUI::Draw_CenteredString(HMI_data.PopupTxt_Color, 240, amsg2);
  if (button) DWINUI::Draw_IconWB(button, 86, 280);
}

template<typename T, typename U>
void DWIN_Show_Popup(const uint8_t icon, T amsg1=nullptr, U amsg2=nullptr, uint8_t button=0) {
  DWIN_Draw_Popup(icon, amsg1, amsg2, button);
  DWIN_UpdateLCD();
}

template<typename T, typename U>
void DWIN_Popup_Confirm(const uint8_t icon, T amsg1, U amsg2) {
  HMI_SaveProcessID(WaitResponse);
  DWIN_Draw_Popup(icon, amsg1, amsg2, ICON_Confirm_E);  // Button Confirm
  DWIN_UpdateLCD();
}

void DWIN_Popup_Continue(const uint8_t icon, FSTR_P const fmsg1, FSTR_P const fmsg2);

void DWIN_Popup_ConfirmCancel(const uint8_t icon, FSTR_P const fmsg2);
