/**
 * Modern Vintage CNC Firmware
*/

/**
 * feature/pause.cpp - Pause feature support functions
 * This may be combined with related G-codes if features are consolidated.
 */

#include "../inc/mvCNCConfigPre.h"

#if ENABLED(ADVANCED_PAUSE_FEATURE)

//#define DEBUG_PAUSE_RESUME

#include "../mvCNCCore.h"
#include "../gcode/gcode.h"
#include "../module/motion.h"
#include "../module/planner.h"
#include "../module/stepper.h"
#include "../module/jobcounter.h"
#include "../module/fan_control.h"

#if ENABLED(AUTO_BED_LEVELING_UBL)
  #include "bedlevel/bedlevel.h"
#endif

#if ENABLED(FWRETRACT)
  #include "fwretract.h"
#endif

#if HAS_FILAMENT_SENSOR
  #include "runout.h"
#endif

#if ENABLED(HOST_ACTION_COMMANDS)
  #include "host_actions.h"
#endif

#if ENABLED(EXTENSIBLE_UI)
  #include "../lcd/extui/ui_api.h"
#elif ENABLED(DWIN_CREALITY_LCD_ENHANCED)
  #include "../lcd/e3v2/proui/dwin.h"
#endif

#include "../lcd/mvcncui.h"

#if HAS_BUZZER
  #include "../libs/buzzer.h"
#endif

#if ENABLED(POWER_LOSS_RECOVERY)
  #include "powerloss.h"
#endif

#include "../libs/nozzle.h"
#include "pause.h"

#define DEBUG_OUT ENABLED(DEBUG_PAUSE_RESUME)
#include "../core/debug_out.h"

// private:

static xyze_pos_t resume_position;

#if M600_PURGE_MORE_RESUMABLE
  PauseMenuResponse pause_menu_response;
  PauseMode pause_mode = PAUSE_MODE_PAUSE_PRINT;
#endif

fil_change_settings_t fc_settings[EXTRUDERS];

#if ENABLED(SDSUPPORT)
  #include "../sd/cardreader.h"
#endif

#if ENABLED(EMERGENCY_PARSER)
  #define _PMSG(L) L##_M108
#else
  #define _PMSG(L) L##_LCD
#endif

#if HAS_BUZZER
  static void impatient_beep(const int8_t max_beep_count, const bool restart=false) {

    if (TERN0(HAS_MVCNCUI_MENU, pause_mode == PAUSE_MODE_PAUSE_PRINT)) return;

    static millis_t next_buzz = 0;
    static int8_t runout_beep = 0;

    if (restart) next_buzz = runout_beep = 0;

    const bool always = max_beep_count < 0;

    const millis_t ms = millis();
    if (ELAPSED(ms, next_buzz)) {
      if (always || runout_beep < max_beep_count + 5) { // Only beep as long as we're supposed to
        next_buzz = ms + ((always || runout_beep < max_beep_count) ? 1000 : 500);
        BUZZ(50, 880 - (runout_beep & 1) * 220);
        runout_beep++;
      }
    }
  }
  inline void first_impatient_beep(const int8_t max_beep_count) { impatient_beep(max_beep_count, true); }
#else
  inline void impatient_beep(const int8_t, const bool=false) {}
  inline void first_impatient_beep(const int8_t) {}
#endif

/**
 * Ensure a safe temperature for extrusion
 *
 * - Fail if the TARGET temperature is too low
 * - Display LCD placard with temperature status
 * - Return when heating is done or aborted
 *
 * Returns 'true' if heating was completed, 'false' for abort
 */
static bool ensure_safe_temperature(const bool wait=true, const PauseMode mode=PAUSE_MODE_SAME) {
  DEBUG_SECTION(est, "ensure_safe_temperature", true);
  DEBUG_ECHOLNPGM("... wait:", wait, " mode:", mode);

  #if ENABLED(PREVENT_COLD_EXTRUSION)
    if (!DEBUGGING(DRYRUN) && thermalManager.targetTooColdToExtrude(active_extruder))
      thermalManager.setTargetHotend(thermalManager.extrude_min_temp, active_extruder);
  #endif

  ui.pause_show_message(PAUSE_MESSAGE_HEATING, mode); UNUSED(mode);

  if (wait) return thermalManager.wait_for_hotend(active_extruder);

  // Allow interruption by Emergency Parser M108
  wait_for_heatup = TERN1(PREVENT_COLD_EXTRUSION, !thermalManager.allow_cold_extrude);
  while (wait_for_heatup && ABS(thermalManager.wholeDegHotend(active_extruder) - thermalManager.degTargetHotend(active_extruder)) > (TEMP_WINDOW))
    idle();
  wait_for_heatup = false;

  #if ENABLED(PREVENT_COLD_EXTRUSION)
    // A user can cancel wait-for-heating with M108
    if (!DEBUGGING(DRYRUN) && thermalManager.targetTooColdToExtrude(active_extruder)) {
      SERIAL_ECHO_MSG(STR_ERR_HOTEND_TOO_COLD);
      return false;
    }
  #endif

  return true;
}

/**
 * Load filament into the hotend
 *
 * - Fail if the a safe temperature was not reached
 * - If pausing for confirmation, wait for a click or M108
 * - Show "wait for load" placard
 * - Load and purge filament
 * - Show "Purge more" / "Continue" menu
 * - Return when "Continue" is selected
 *
 * Returns 'true' if load was completed, 'false' for abort
 */
bool load_filament(const_float_t slow_load_length/*=0*/, const_float_t fast_load_length/*=0*/, const_float_t purge_length/*=0*/, const int8_t max_beep_count/*=0*/,
                   const bool show_lcd/*=false*/, const bool pause_for_user/*=false*/,
                   const PauseMode mode/*=PAUSE_MODE_PAUSE_PRINT*/
                   DXC_ARGS
) {
  DEBUG_SECTION(lf, "load_filament", true);
  DEBUG_ECHOLNPGM("... slowlen:", slow_load_length, " fastlen:", fast_load_length, " purgelen:", purge_length, " maxbeep:", max_beep_count, " showlcd:", show_lcd, " pauseforuser:", pause_for_user, " pausemode:", mode DXC_SAY);

  if (!ensure_safe_temperature(false, mode)) {
    if (show_lcd) ui.pause_show_message(PAUSE_MESSAGE_STATUS, mode);
    return false;
  }

  if (pause_for_user) {
    if (show_lcd) ui.pause_show_message(PAUSE_MESSAGE_INSERT, mode);
    SERIAL_ECHO_MSG(_PMSG(STR_FILAMENT_CHANGE_INSERT));

    first_impatient_beep(max_beep_count);

    KEEPALIVE_STATE(PAUSED_FOR_USER);
    wait_for_user = true;    // LCD click or M108 will clear this

    TERN_(EXTENSIBLE_UI, ExtUI::onUserConfirmRequired(F("Load Filament")));

    #if ENABLED(HOST_PROMPT_SUPPORT)
      const char tool = '0' + TERN0(MULTI_FILAMENT_SENSOR, active_extruder);
      hostui.prompt_do(PROMPT_USER_CONTINUE, F("Load Filament T"), tool, FPSTR(CONTINUE_STR));
    #endif

    while (wait_for_user) {
      impatient_beep(max_beep_count);
      #if BOTH(FILAMENT_CHANGE_RESUME_ON_INSERT, FILAMENT_RUNOUT_SENSOR)
        #if ENABLED(MULTI_FILAMENT_SENSOR)
          #define _CASE_INSERTED(N) case N-1: if (READ(FIL_RUNOUT##N##_PIN) != FIL_RUNOUT##N##_STATE) wait_for_user = false; break;
          switch (active_extruder) {
            REPEAT_1(NUM_RUNOUT_SENSORS, _CASE_INSERTED)
          }
        #else
          if (READ(FIL_RUNOUT_PIN) != FIL_RUNOUT_STATE) wait_for_user = false;
        #endif
      #endif
      idle_no_sleep();
    }
  }

  if (show_lcd) ui.pause_show_message(PAUSE_MESSAGE_LOAD, mode);

  #if ENABLED(DUAL_X_CARRIAGE)
    const int8_t saved_ext        = active_extruder;
    const bool saved_ext_dup_mode = extruder_duplication_enabled;
    set_duplication_enabled(false, DXC_ext);
  #endif

  TERN_(BELTPRINTER, do_blocking_move_to_xy(0.00, 50.00));

  // Slow Load filament
  if (slow_load_length) unscaled_e_move(slow_load_length, FILAMENT_CHANGE_SLOW_LOAD_FEEDRATE);

  // Fast Load Filament
  if (fast_load_length) {
    #if FILAMENT_CHANGE_FAST_LOAD_ACCEL > 0
      const float saved_acceleration = planner.settings.retract_acceleration;
      planner.settings.retract_acceleration = FILAMENT_CHANGE_FAST_LOAD_ACCEL;
    #endif

    unscaled_e_move(fast_load_length, FILAMENT_CHANGE_FAST_LOAD_FEEDRATE);

    #if FILAMENT_CHANGE_FAST_LOAD_ACCEL > 0
      planner.settings.retract_acceleration = saved_acceleration;
    #endif
  }

  #if ENABLED(DUAL_X_CARRIAGE)      // Tie the two extruders movement back together.
    set_duplication_enabled(saved_ext_dup_mode, saved_ext);
  #endif

  #if ENABLED(ADVANCED_PAUSE_CONTINUOUS_PURGE)

    if (show_lcd) ui.pause_show_message(PAUSE_MESSAGE_PURGE);

    TERN_(EXTENSIBLE_UI, ExtUI::onUserConfirmRequired(GET_TEXT_F(MSG_FILAMENT_CHANGE_PURGE)));
    TERN_(HOST_PROMPT_SUPPORT, hostui.prompt_do(PROMPT_USER_CONTINUE, GET_TEXT_F(MSG_FILAMENT_CHANGE_PURGE), FPSTR(CONTINUE_STR)));
    wait_for_user = true; // A click or M108 breaks the purge_length loop
    for (float purge_count = purge_length; purge_count > 0 && wait_for_user; --purge_count)
      unscaled_e_move(1, ADVANCED_PAUSE_PURGE_FEEDRATE);
    wait_for_user = false;

  #else

    do {
      if (purge_length > 0) {
        // "Wait for filament purge"
        if (show_lcd) ui.pause_show_message(PAUSE_MESSAGE_PURGE);

        // Extrude filament to get into hotend
        unscaled_e_move(purge_length, ADVANCED_PAUSE_PURGE_FEEDRATE);
      }

      TERN_(HOST_PROMPT_SUPPORT, hostui.filament_load_prompt()); // Initiate another host prompt.

      #if M600_PURGE_MORE_RESUMABLE
        if (show_lcd) {
          // Show "Purge More" / "Resume" menu and wait for reply
          KEEPALIVE_STATE(PAUSED_FOR_USER);
          wait_for_user = false;
          #if EITHER(HAS_MVCNCUI_MENU, DWIN_CREALITY_LCD_ENHANCED)
            ui.pause_show_message(PAUSE_MESSAGE_OPTION); // Also sets PAUSE_RESPONSE_WAIT_FOR
          #else
            pause_menu_response = PAUSE_RESPONSE_WAIT_FOR;
          #endif
          while (pause_menu_response == PAUSE_RESPONSE_WAIT_FOR) idle_no_sleep();
        }
      #endif

      // Keep looping if "Purge More" was selected
    } while (TERN0(M600_PURGE_MORE_RESUMABLE, pause_menu_response == PAUSE_RESPONSE_EXTRUDE_MORE));

  #endif
  TERN_(HOST_PROMPT_SUPPORT, hostui.prompt_end());

  return true;
}

// public:

/**
 * Pause procedure
 *
 * - Abort if already paused
 * - Send host action for pause, if configured
 * - Abort if TARGET temperature is too low
 * - Display "wait for start of filament change" (if a length was specified)
 * - Initial retract, if current temperature is hot enough
 * - Park the nozzle at the given position
 * - Call unload_filament (if a length was specified)
 *
 * Return 'true' if pause was completed, 'false' for abort
 */
uint8_t did_pause_print = 0;

bool pause_print(const_float_t retract, const xyz_pos_t &park_point, const bool show_lcd/*=false*/, const_float_t unload_length/*=0*/ DXC_ARGS) {
  DEBUG_SECTION(pp, "pause_print", true);
  DEBUG_ECHOLNPGM("... park.x:", park_point.x, " y:", park_point.y, " z:", park_point.z, " unloadlen:", unload_length, " showlcd:", show_lcd DXC_SAY);

  UNUSED(show_lcd);

  if (did_pause_print) return false; // already paused

  #if ENABLED(HOST_ACTION_COMMANDS)
    #ifdef ACTION_ON_PAUSED
      hostui.paused();
    #elif defined(ACTION_ON_PAUSE)
      hostui.pause();
    #endif
  #endif

  TERN_(HOST_PROMPT_SUPPORT, hostui.prompt_open(PROMPT_INFO, F("Pause"), FPSTR(DISMISS_STR)));

  // Indicate that the cnc is paused
  ++did_pause_print;

  // Pause the CNC job and timer
  #if ENABLED(SDSUPPORT)
    const bool was_sd_printing = IS_SD_PRINTING();
    if (was_sd_printing) {
      card.pauseSDPrint();
      ++did_pause_print; // Indicate SD pause also
    }
  #endif

  print_job_timer.pause();

  // Save current position
  resume_position = current_position;

  // Will the nozzle be parking?
  const bool do_park = !axes_should_home();

  #if ENABLED(POWER_LOSS_RECOVERY)
    // Save PLR info in case the power goes out while parked
    const float park_raise = do_park ? nozzle.park_mode_0_height(park_point.z) - current_position.z : POWER_LOSS_ZRAISE;
    if (was_sd_printing && recovery.enabled) recovery.save(true, park_raise, do_park);
  #endif

  // Wait for buffered blocks to complete
  planner.synchronize();

  #if ENABLED(ADVANCED_PAUSE_FANS_PAUSE) && HAS_FAN
    thermalManager.set_fans_paused(true);
  #endif

  // If axes don't need to home then the nozzle can park
  if (do_park) nozzle.park(0, park_point); // Park the nozzle by doing a Minimum Z Raise followed by an XY Move

  #if ENABLED(DUAL_X_CARRIAGE)
    const int8_t saved_ext        = active_extruder;
    const bool saved_ext_dup_mode = extruder_duplication_enabled;
    set_duplication_enabled(false, DXC_ext);
  #endif

  #if ENABLED(DUAL_X_CARRIAGE)
    set_duplication_enabled(saved_ext_dup_mode, saved_ext);
  #endif

  return true;
}

/**
 * For Paused Print:
 * - Show "Press button (or M108) to resume"
 *
 * For Filament Change:
 * - Show "Insert filament and press button to continue"
 *
 * - Wait for a click before returning
 * - Heaters can time out and must reheat before continuing
 *
 * Used by M125 and M600
 */

void show_continue_prompt(const bool is_reload) {
  DEBUG_SECTION(scp, "pause_print", true);
  DEBUG_ECHOLNPGM("... is_reload:", is_reload);

  ui.pause_show_message(is_reload ? PAUSE_MESSAGE_INSERT : PAUSE_MESSAGE_WAITING);
  SERIAL_ECHO_START();
  SERIAL_ECHOF(is_reload ? F(_PMSG(STR_FILAMENT_CHANGE_INSERT) "\n") : F(_PMSG(STR_FILAMENT_CHANGE_WAIT) "\n"));
}

void wait_for_confirmation(const bool is_reload/*=false*/, const int8_t max_beep_count/*=0*/ DXC_ARGS) {
  DEBUG_SECTION(wfc, "wait_for_confirmation", true);
  DEBUG_ECHOLNPGM("... is_reload:", is_reload, " maxbeep:", max_beep_count DXC_SAY);

  show_continue_prompt(is_reload);

  first_impatient_beep(max_beep_count);

  #if ENABLED(DUAL_X_CARRIAGE)
    const int8_t saved_ext        = active_extruder;
    const bool saved_ext_dup_mode = extruder_duplication_enabled;
    set_duplication_enabled(false, DXC_ext);
  #endif

  // Wait for filament insert by user and press button
  KEEPALIVE_STATE(PAUSED_FOR_USER);
  TERN_(HOST_PROMPT_SUPPORT, hostui.prompt_do(PROMPT_USER_CONTINUE, GET_TEXT_F(MSG_NOZZLE_PARKED), FPSTR(CONTINUE_STR)));
  TERN_(EXTENSIBLE_UI, ExtUI::onUserConfirmRequired(GET_TEXT_F(MSG_NOZZLE_PARKED)));
  wait_for_user = true;    // LCD click or M108 will clear this
  while (wait_for_user) {
    impatient_beep(max_beep_count);

    idle_no_sleep();
  }
  #if ENABLED(DUAL_X_CARRIAGE)
    set_duplication_enabled(saved_ext_dup_mode, saved_ext);
  #endif
}

/**
 * Resume or Start print procedure
 *
 * - If not paused, do nothing and return
 * - Reset heater idle timers
 * - Load filament if specified, but only if:
 *   - a nozzle timed out, or
 *   - the nozzle is already heated.
 * - Display "wait for print to resume"
 * - Retract to prevent oozing
 * - Move the nozzle back to resume_position
 * - Unretract
 * - Re-prime the nozzle...
 *   -  FWRETRACT: Recover/prime from the prior G10.
 *   - !FWRETRACT: Retract by resume_position.e, if negative.
 *                 Not sure how this logic comes into use.
 * - Sync the planner E to resume_position.e
 * - Send host action for resume, if configured
 * - Resume the current SD CNC job, if any
 */
void resume_print(const_float_t slow_load_length/*=0*/, const_float_t fast_load_length/*=0*/, const_float_t purge_length/*=ADVANCED_PAUSE_PURGE_LENGTH*/, const int8_t max_beep_count/*=0*/, const celsius_t targetTemp/*=0*/ DXC_ARGS) {
  DEBUG_SECTION(rp, "resume_print", true);
  DEBUG_ECHOLNPGM("... slowlen:", slow_load_length, " fastlen:", fast_load_length, " purgelen:", purge_length, " maxbeep:", max_beep_count, " targetTemp:", targetTemp DXC_SAY);

  /*
  SERIAL_ECHOLNPGM(
    "start of resume_print()\ndual_x_carriage_mode:", dual_x_carriage_mode,
    "\nextruder_duplication_enabled:", extruder_duplication_enabled,
    "\nactive_extruder:", active_extruder,
    "\n"
  );
  //*/

  if (!did_pause_print) return;

  ui.pause_show_message(PAUSE_MESSAGE_RESUME);

  if (!axes_should_home()) {
    // Move XY back to saved position
    destination.set(resume_position.x, resume_position.y, current_position.z, current_position.e);
    prepare_internal_move_to_destination(SPINDLE_PARK_XY_FEEDRATE);

    // Move Z back to saved position
    destination.z = resume_position.z;
    prepare_internal_move_to_destination(SPINDLE_PARK_Z_FEEDRATE);
  }

  #if ENABLED(AUTO_BED_LEVELING_UBL)
    const bool leveling_was_enabled = planner.leveling_active; // save leveling state
    set_bed_leveling_enabled(false);  // turn off leveling
  #endif

  ui.pause_show_message(PAUSE_MESSAGE_STATUS);

  #ifdef ACTION_ON_RESUMED
    hostui.resumed();
  #elif defined(ACTION_ON_RESUME)
    hostui.resume();
  #endif

  --did_pause_print;

  TERN_(HOST_PROMPT_SUPPORT, hostui.prompt_open(PROMPT_INFO, F("Resuming"), FPSTR(DISMISS_STR)));

  // Resume the CNC job timer if it was running
  if (print_job_timer.isPaused()) print_job_timer.start();

  #if ENABLED(SDSUPPORT)
    if (did_pause_print) {
      --did_pause_print;
      card.startOrResumeFilePrinting();
      // Write PLR now to update the z axis value
      TERN_(POWER_LOSS_RECOVERY, if (recovery.enabled) recovery.save(true));
    }
  #endif

  #if ENABLED(ADVANCED_PAUSE_FANS_PAUSE) && HAS_FAN
    thermalManager.set_fans_paused(false);
  #endif

  TERN_(HAS_STATUS_MESSAGE, ui.reset_status());
  TERN_(HAS_MVCNCUI_MENU, ui.return_to_status());
  TERN_(DWIN_CREALITY_LCD_ENHANCED, HMI_ReturnScreen());
}

#endif // ADVANCED_PAUSE_FEATURE
