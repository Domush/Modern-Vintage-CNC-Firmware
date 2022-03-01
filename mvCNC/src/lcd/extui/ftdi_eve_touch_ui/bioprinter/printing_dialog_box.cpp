/*******************************
 * bio_cutting_dialog_box.cpp *
 *******************************/

/****************************************************************************
 *   Written By Mark Pelletier  2017 - Aleph Objects, Inc.                  *
 *   Written By Marcio Teixeira 2018 - Aleph Objects, Inc.                  *
 *                                                                          *
 *   This program is free software: you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation, either version 3 of the License, or      *
 *   (at your option) any later version.                                    *
 *                                                                          *
 *   This program is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU General Public License for more details.                           *
 *                                                                          *
 *   To view a copy of the GNU General Public License, go to the following  *
 *   location: <https://www.gnu.org/licenses/>.                             *
 ****************************************************************************/

#include "../config.h"
#include "../screens.h"

#ifdef FTDI_BIO_CUTTING_DIALOG_BOX

using namespace FTDI;
using namespace ExtUI;
using namespace Theme;

#define GRID_COLS 2
#define GRID_ROWS 9

void BioCuttingDialogBox::draw_status_message(draw_mode_t what, const char *message) {
  if (what & BACKGROUND) {
    CommandProcessor cmd;
    cmd.cmd(COLOR_RGB(bg_text_enabled))
       .tag(0);
    draw_text_box(cmd, BTN_POS(1,2), BTN_SIZE(2,2), message, OPT_CENTER, font_large);
  }
}

void BioCuttingDialogBox::draw_progress(draw_mode_t what) {
  if (what & FOREGROUND) {
    CommandProcessor cmd;
    cmd.font(font_large)
       .text(BTN_POS(1,1), BTN_SIZE(2,2), jobRunning() ? F("CNCing...") : F("Finished."))
       .tag(1)
       .font(font_xlarge);

    draw_circular_progress(cmd, BTN_POS(1,4), BTN_SIZE(2,3), getProgress_percent(), theme_dark, theme_darkest);
  }
}

void BioCuttingDialogBox::draw_time_remaining(draw_mode_t what) {
  if (what & FOREGROUND) {
    const uint32_t elapsed = getProgress_seconds_elapsed();
    const uint8_t hrs = elapsed/3600;
    const uint8_t min = (elapsed/60)%60;

    char time_str[10];
    sprintf_P(time_str, PSTR("%02dh %02dm"), hrs, min);

    CommandProcessor cmd;
    cmd.font(font_large)
       .text(BTN_POS(1,7), BTN_SIZE(2,2), time_str);
  }
}

void BioCuttingDialogBox::draw_interaction_buttons(draw_mode_t what) {
  if (what & FOREGROUND) {
    CommandProcessor cmd;
    cmd.colors(normal_btn)
       .font(font_medium)
       .colors(jobRunning() ? action_btn : normal_btn)
       .tag(2).button(BTN_POS(1,9), BTN_SIZE(1,1), F("Menu"))
       .enabled(jobRunning() ? TERN0(SDSUPPORT, jobRunningFromMedia()) : 1)
       .tag(3)
       .colors(jobRunning() ? normal_btn : action_btn)
       .button(BTN_POS(2,9), BTN_SIZE(1,1), jobRunning() ? F("Cancel") : F("Back"));
  }
}

void BioCuttingDialogBox::onRedraw(draw_mode_t what) {
  if (what & FOREGROUND) {
    draw_progress(FOREGROUND);
    draw_time_remaining(FOREGROUND);
    draw_interaction_buttons(FOREGROUND);
  }
}

bool BioCuttingDialogBox::onTouchEnd(uint8_t tag) {
  switch (tag) {
    case 1: GOTO_SCREEN(FeedratePercentScreen); break;
    case 2: GOTO_SCREEN(TuneMenu); break;
    case 3:
      if (jobRunning())
        GOTO_SCREEN(ConfirmAbortPrintDialogBox);
      else
        GOTO_SCREEN(StatusScreen);
      break;
    default: return false;
  }
  return true;
}

void BioCuttingDialogBox::setStatusMessage(FSTR_P message) {
  char buff[strlen_P((const char*)message)+1];
  strcpy_P(buff, (const char*) message);
  setStatusMessage(buff);
}

void BioCuttingDialogBox::setStatusMessage(const char *message) {
  CommandProcessor cmd;
  cmd.cmd(CMD_DLSTART)
     .cmd(CLEAR_COLOR_RGB(bg_color))
     .cmd(CLEAR(true,true,true));

  draw_status_message(BACKGROUND, message);
  draw_progress(BACKGROUND);
  draw_time_remaining(BACKGROUND);
  draw_interaction_buttons(BACKGROUND);
  storeBackground();

  #if ENABLED(TOUCH_UI_DEBUG)
    SERIAL_ECHO_MSG("New status message: ", message);
  #endif

  if (AT_SCREEN(BioCuttingDialogBox))
    current_screen.onRefresh();
}

void BioCuttingDialogBox::onIdle() {
  reset_menu_timeout();
  if (refresh_timer.elapsed(STATUS_UPDATE_INTERVAL)) {
    onRefresh();
    refresh_timer.start();
  }
  BaseScreen::onIdle();
}

void BioCuttingDialogBox::show() {
  GOTO_SCREEN(BioCuttingDialogBox);
}

#endif // FTDI_BIO_CUTTING_DIALOG_BOX
