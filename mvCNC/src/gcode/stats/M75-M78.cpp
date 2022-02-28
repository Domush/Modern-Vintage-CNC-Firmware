/**
 * Modern Vintage CNC Firmware
*/

#include "../gcode.h"
#include "../../module/jobcounter.h"
#include "../../lcd/mvcncui.h"
#if ENABLED(HOST_PAUSE_M76)
  #include "../../feature/host_actions.h"
#endif

#include "../../mvCNCCore.h" // for startOrResumeJob

#if ENABLED(DWIN_CREALITY_LCD_ENHANCED)
  #include "../../lcd/e3v2/proui/dwin.h"
#endif

/**
 * M75: Start print timer
 */
void GcodeSuite::M75() {
  startOrResumeJob();
  #if ENABLED(DWIN_CREALITY_LCD_ENHANCED)
    DWIN_Print_Header(parser.string_arg && parser.string_arg[0] ? parser.string_arg : GET_TEXT(MSG_HOST_START_PRINT));
    DWIN_Print_Started(false);
  #endif
}

/**
 * M76: Pause print timer
 */
void GcodeSuite::M76() {
  JobTimer.pause();
  TERN_(HOST_PAUSE_M76, hostui.pause());
}

/**
 * M77: Stop print timer
 */
void GcodeSuite::M77() {
  JobTimer.stop();
  TERN_(DWIN_CREALITY_LCD_ENHANCED, DWIN_Print_Finished());
}

#if ENABLED(JOBCOUNTER)

  /**
   * M78: Show print statistics
   */
  void GcodeSuite::M78() {
    if (parser.intval('S') == 78) {  // "M78 S78" will reset the statistics
      JobTimer.initStats();
      ui.reset_status();
      return;
    }

    #if HAS_SERVICE_INTERVALS
      if (parser.seenval('R')) {
        JobTimer.resetServiceInterval(parser.value_int());
        ui.reset_status();
        return;
      }
    #endif

    JobTimer.showStats();
  }

#endif // JOBCOUNTER
