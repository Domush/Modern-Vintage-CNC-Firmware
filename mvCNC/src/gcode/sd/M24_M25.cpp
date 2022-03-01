/**
 * Modern Vintage CNC Firmware
*/

#include "../../inc/mvCNCConfig.h"

#if ENABLED(SDSUPPORT)

#include "../gcode.h"
#include "../../sd/cardreader.h"
#include "../../module/jobcounter.h"
#include "../../lcd/mvcncui.h"

#if ENABLED(PARK_HEAD_ON_PAUSE)
  #include "../../feature/pause.h"
#endif

#if ENABLED(HOST_ACTION_COMMANDS)
  #include "../../feature/host_actions.h"
#endif

#if ENABLED(POWER_LOSS_RECOVERY)
  #include "../../feature/powerloss.h"
#endif

#if ENABLED(DGUS_LCD_UI_MKS)
  #include "../../lcd/extui/dgus/DGUSDisplayDef.h"
#endif

#include "../../mvCNCCore.h" // for startOrResumeJob

/**
 * M24: Start or Resume SD Print
 */
void GcodeSuite::M24() {

  #if ENABLED(DGUS_LCD_UI_MKS)
    if ((JobTimer.isPaused() || JobTimer.isRunning()) && !parser.seen("ST"))
      MKS_resume_print_move();
  #endif

  #if ENABLED(POWER_LOSS_RECOVERY)
    if (parser.seenval('S')) card.setIndex(parser.value_long());
    if (parser.seenval('T')) JobTimer.resume(parser.value_long());
  #endif

  #if ENABLED(PARK_HEAD_ON_PAUSE)
    if (did_pause_print) {
      resume_print(); // will call JobTimer.start()
      return;
    }
  #endif

  if (card.isFileOpen()) {
    card.startOrResumeFileCutting();            // SD card will now be read for commands
    startOrResumeJob();               // Start (or resume) the CNC job timer
    TERN_(POWER_LOSS_RECOVERY, recovery.prepare());
  }

  #if ENABLED(HOST_ACTION_COMMANDS)
    #ifdef ACTION_ON_RESUME
      hostui.resume();
    #endif
    TERN_(HOST_PROMPT_SUPPORT, hostui.prompt_open(PROMPT_INFO, F("Resuming SD"), FPSTR(DISMISS_STR)));
  #endif

  ui.reset_status();
}

/**
 * M25: Pause SD Print
 *
 * With PARK_HEAD_ON_PAUSE:
 *   Invoke M125 to store the current position and move to the park
 *   position. M24 will move the head back before resuming the print.
 */
void GcodeSuite::M25() {

  #if ENABLED(PARK_HEAD_ON_PAUSE)

    M125();

  #else

    // Set initial pause flag to prevent more commands from landing in the queue while we try to pause
    #if ENABLED(SDSUPPORT)
      if (IS_SD_JOB_RUNNING()) card.pauseSDJob();
    #endif

    #if ENABLED(POWER_LOSS_RECOVERY) && DISABLED(DGUS_LCD_UI_MKS)
      if (recovery.enabled) recovery.save(true);
    #endif

    JobTimer.pause();

    TERN_(DGUS_LCD_UI_MKS, MKS_pause_print_move());

    IF_DISABLED(DWIN_CREALITY_LCD, ui.reset_status());

    #if ENABLED(HOST_ACTION_COMMANDS)
      TERN_(HOST_PROMPT_SUPPORT, hostui.prompt_open(PROMPT_PAUSE_RESUME, F("Pause SD"), F("Resume")));
      #ifdef ACTION_ON_PAUSE
        hostui.pause();
      #endif
    #endif

  #endif
}

#endif // SDSUPPORT
