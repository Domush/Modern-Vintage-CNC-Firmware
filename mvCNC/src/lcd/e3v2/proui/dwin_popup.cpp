/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
/**
 * DWIN UI Enhanced implementation
 * Author: Miguel A. Risco-Castillo (MRISCOC)
 * Version: 3.10.1
 * Date: 2022/01/21
 *
 * Based on the original code provided by Creality under GPL
 */

#include "../../../inc/mvCNCConfigPre.h"

#if ENABLED(DWIN_CREALITY_LCD_ENHANCED)

#include "dwin.h"
#include "dwin_popup.h"

void Draw_Select_Highlight(const bool sel) {
  HMI_flag.select_flag = sel;
  const uint16_t c1 = sel ? HMI_data.Highlight_Color : HMI_data.PopupBg_color,
                 c2 = sel ? HMI_data.PopupBg_color : HMI_data.Highlight_Color;
  DWIN_Draw_Rectangle(0, c1, 25, 279, 126, 318);
  DWIN_Draw_Rectangle(0, c1, 24, 278, 127, 319);
  DWIN_Draw_Rectangle(0, c2, 145, 279, 246, 318);
  DWIN_Draw_Rectangle(0, c2, 144, 278, 247, 319);
}

void DWIN_Popup_Continue(const uint8_t icon, FSTR_P const fmsg1, FSTR_P const fmsg2) {
  HMI_SaveProcessID(WaitResponse);
  DWIN_Draw_Popup(icon, fmsg1, fmsg2, ICON_Continue_E);  // Button Continue
  DWIN_UpdateLCD();
}

void DWIN_Popup_ConfirmCancel(const uint8_t icon, FSTR_P const fmsg2) {
  DWIN_Draw_Popup(ICON_BLTouch, F("Please confirm"), fmsg2);
  DWINUI::Draw_IconWB(ICON_Confirm_E, 26, 280);
  DWINUI::Draw_IconWB(ICON_Cancel_E, 146, 280);
  Draw_Select_Highlight(true);
  DWIN_UpdateLCD();
}

#endif // DWIN_CREALITY_LCD_ENHANCED
