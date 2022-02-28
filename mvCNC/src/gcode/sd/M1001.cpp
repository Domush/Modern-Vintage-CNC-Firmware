/**
 * Modern Vintage CNC Firmware
*/

#include "../../inc/mvCNCConfig.h"

#if ENABLED(SDSUPPORT)

#include "../gcode.h"
#include "../../module/planner.h"
#include "../../module/jobcounter.h"
#include "../../module/fan_control.h"
#include "../../sd/cardreader.h"

#ifdef SD_FINISHED_RELEASECOMMAND
  #include "../queue.h"
#endif

#if EITHER(LCD_SET_PROGRESS_MANUALLY, SD_REPRINT_LAST_SELECTED_FILE)
  #include "../../lcd/mvcncui.h"
#endif

#if ENABLED(POWER_LOSS_RECOVERY)
  #include "../../feature/powerloss.h"
#endif

#if HAS_LEDS_OFF_FLAG
  #include "../../mvCNCCore.h" // for wait_for_user_response()
  #include "../../feature/leds/cnc_event_leds.h"
#endif

#if ENABLED(EXTENSIBLE_UI)
  #include "../../lcd/extui/ui_api.h"
#elif ENABLED(DWIN_CREALITY_LCD_ENHANCED)
  #include "../../lcd/e3v2/proui/dwin.h"
#endif

#if ENABLED(HOST_ACTION_COMMANDS)
  #include "../../feature/host_actions.h"
#endif

#ifndef PE_LEDS_COMPLETED_TIME
  #define PE_LEDS_COMPLETED_TIME (30*60)
#endif

/**
 * M1001: Execute actions for SD print completion
 */
void GcodeSuite::M1001() {
  planner.synchronize();

  // SD CNCing is finished when the queue reaches M1001
  card.flag.sdprinting = card.flag.sdprintdone = false;

  // If there's another auto#.g file to run...
  if (TERN(NO_SD_AUTOSTART, false, card.autofile_check())) return;

  // Purge the recovery file...
  TERN_(POWER_LOSS_RECOVERY, recovery.purge());

  // Report total print time
  const bool long_print = JobTimer.duration() > 60;
  if (long_print) process_subcommands_now(F("M31"));

  // Stop the CNC job timer
  process_subcommands_now(F("M77"));

  // Set the progress bar "done" state
  TERN_(LCD_SET_PROGRESS_MANUALLY, ui.set_progress_done());

  // Announce SD file completion
  {
    PORT_REDIRECT(SerialMask::All);
    SERIAL_ECHOLNPGM(STR_FILE_PRINTED);
  }

  // Update the status LED color
  #if HAS_LEDS_OFF_FLAG
    if (long_print) {
      cncEventLEDs.onPrintCompleted();
      TERN_(EXTENSIBLE_UI, ExtUI::onUserConfirmRequired(GET_TEXT_F(MSG_PRINT_DONE)));
      TERN_(HOST_PROMPT_SUPPORT, hostui.prompt_do(PROMPT_USER_CONTINUE, GET_TEXT_F(MSG_PRINT_DONE), FPSTR(CONTINUE_STR)));
      TERN_(HAS_RESUME_CONTINUE, wait_for_user_response(SEC_TO_MS(TERN(HAS_MVCNCUI_MENU, PE_LEDS_COMPLETED_TIME, 30))));
      cncEventLEDs.onResumeAfterWait();
    }
  #endif

  // Inject SD_FINISHED_RELEASECOMMAND, if any
  #ifdef SD_FINISHED_RELEASECOMMAND
    process_subcommands_now(F(SD_FINISHED_RELEASECOMMAND));
  #endif

  TERN_(EXTENSIBLE_UI, ExtUI::onPrintFinished());
  TERN_(DWIN_CREALITY_LCD_ENHANCED, DWIN_Print_Finished());

  // Re-select the last printed file in the UI
  TERN_(SD_REPRINT_LAST_SELECTED_FILE, ui.reselect_last_file());
}

#endif // SDSUPPORT
