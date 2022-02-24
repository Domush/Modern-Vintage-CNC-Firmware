/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
/**
 * DWIN CNC Stats page
 * Author: Miguel A. Risco-Castillo (MRISCOC)
 * Version: 1.1
 * Date: 2022/01/09
 *
 * Based on the original code provided by Creality under GPL
 */

#include "../../../inc/mvCNCConfigPre.h"

#if BOTH(DWIN_CREALITY_LCD_ENHANCED, JOBCOUNTER)

#include "printstats.h"

#include "../../../core/types.h"
#include "../../mvcncui.h"
#include "../../../module/jobcounter.h"
#include "dwin_lcd.h"
#include "dwinui.h"
#include "dwin_popup.h"
#include "dwin.h"

CNCStatsClass CNCStats;

void CNCStatsClass::Draw() {
  char buf[50] = "";
  char str[30] = "";
  constexpr int8_t MRG = 30;

  Title.ShowCaption(GET_TEXT_F(MSG_INFO_STATS_MENU));
  DWINUI::ClearMenuArea();
  Draw_Popup_Bkgd();
  DWINUI::Draw_Icon(ICON_Continue_E, 86, 250);
  printStatistics ps = print_job_timer.getStats();

  sprintf_P(buf, PSTR(S_FMT ": %i"), GET_TEXT(MSG_INFO_PRINT_COUNT), ps.totalPrints);
  DWINUI::Draw_String(MRG, 80, buf);
  sprintf_P(buf, PSTR(S_FMT ": %i"), GET_TEXT(MSG_INFO_COMPLETED_PRINTS), ps.finishedPrints);
  DWINUI::Draw_String(MRG, 100, buf);
  duration_t(print_job_timer.getStats().printTime).toDigital(str, true);
  sprintf_P(buf, PSTR(S_FMT ": %s"), GET_TEXT(MSG_INFO_PRINT_TIME), str);
  DWINUI::Draw_String(MRG, 120, buf);
  duration_t(print_job_timer.getStats().longestPrint).toDigital(str, true);
  sprintf_P(buf, PSTR(S_FMT ": %s"), GET_TEXT(MSG_INFO_PRINT_LONGEST), str);
  DWINUI::Draw_String(MRG, 140, buf);
  sprintf_P(buf, PSTR(S_FMT ": %s m"), GET_TEXT(MSG_INFO_PRINT_FILAMENT), dtostrf(ps.filamentUsed / 1000, 1, 2, str));
  DWINUI::Draw_String(MRG, 160, buf);
}

void CNCStatsClass::Reset() {
  print_job_timer.initStats();
  HMI_AudioFeedback();
}

#endif // DWIN_CREALITY_LCD_ENHANCED && JOBCOUNTER
