/**
 * Modern Vintage CNC Firmware
*/

/**
 * feature/powerloss.cpp - Resume an SD print after power-loss
 */

#include "../inc/mvCNCConfigPre.h"

#if ENABLED(POWER_LOSS_RECOVERY)

#include "powerloss.h"
#include "../core/macros.h"

bool CNCJobRecovery::enabled; // Initialized by settings.load()

SdFile CNCJobRecovery::file;
job_recovery_info_t CNCJobRecovery::info;
const char CNCJobRecovery::filename[5] = "/PLR";
uint8_t CNCJobRecovery::queue_index_r;
uint32_t CNCJobRecovery::cmd_sdpos, // = 0
         CNCJobRecovery::sdpos[BUFSIZE];

#if HAS_DWIN_E3V2_BASIC
  bool CNCJobRecovery::dwin_flag; // = false
#endif

#include "../sd/cardreader.h"
#include "../lcd/mvcncui.h"
#include "../gcode/queue.h"
#include "../gcode/gcode.h"
#include "../module/motion.h"
#include "../module/planner.h"
#include "../module/jobcounter.h"
#include "../module/temperature.h"
#include "../core/serial.h"

#define DEBUG_OUT ENABLED(DEBUG_POWER_LOSS_RECOVERY)
#include "../core/debug_out.h"

CNCJobRecovery recovery;

#ifndef POWER_LOSS_PURGE_LEN
  #define POWER_LOSS_PURGE_LEN 0
#endif

#if DISABLED(BACKUP_POWER_SUPPLY)
  #undef POWER_LOSS_RETRACT_LEN   // No retract at outage without backup power
#endif
#ifndef POWER_LOSS_RETRACT_LEN
  #define POWER_LOSS_RETRACT_LEN 0
#endif

/**
 * Clear the recovery info
 */
void CNCJobRecovery::init() { memset(&info, 0, sizeof(info)); }

/**
 * Enable or disable then call changed()
 */
void CNCJobRecovery::enable(const bool onoff) {
  enabled = onoff;
  changed();
}

/**
 * The enabled state was changed:
 *  - Enabled: Purge the job recovery file
 *  - Disabled: Write the job recovery file
 */
void CNCJobRecovery::changed() {
  if (!enabled)
    purge();
  else if (IS_SD_PRINTING())
    save(true);
}

/**
 * Check for CNC Job Recovery during setup()
 *
 * If a saved state exists send 'M1000 S' to initiate job recovery.
 */
void CNCJobRecovery::check() {
  //if (!card.isMounted()) card.mount();
  if (card.isMounted()) {
    load();
    if (!valid()) return cancel();
    queue.inject(F("M1000S"));
  }
}

/**
 * Delete the recovery file and clear the recovery data
 */
void CNCJobRecovery::purge() {
  init();
  card.removeJobRecoveryFile();
}

/**
 * Load the recovery data, if it exists
 */
void CNCJobRecovery::load() {
  if (exists()) {
    open(true);
    (void)file.read(&info, sizeof(info));
    close();
  }
  debug(F("Load"));
}

/**
 * Set info fields that won't change
 */
void CNCJobRecovery::prepare() {
  card.getAbsFilenameInCWD(info.sd_filename);  // SD filename
  cmd_sdpos = 0;
}

/**
 * Save the current machine state to the power-loss recovery file
 */
void CNCJobRecovery::save(const bool force/*=false*/, const float zraise/*=POWER_LOSS_ZRAISE*/, const bool raised/*=false*/) {

  // We don't check IS_SD_PRINTING here so a save may occur during a pause

  #if SAVE_INFO_INTERVAL_MS > 0
    static millis_t next_save_ms; // = 0
    millis_t ms = millis();
  #endif

  #ifndef POWER_LOSS_MIN_Z_CHANGE
    #define POWER_LOSS_MIN_Z_CHANGE 0.05  // Vase-mode-friendly out of the box
  #endif

  // Did Z change since the last call?
  if (force
    #if DISABLED(SAVE_EACH_CMD_MODE)      // Always save state when enabled
      #if SAVE_INFO_INTERVAL_MS > 0       // Save if interval is elapsed
        || ELAPSED(ms, next_save_ms)
      #endif
      // Save if Z is above the last-saved position by some minimum height
      || current_position.z > info.current_position.z + POWER_LOSS_MIN_Z_CHANGE
    #endif
  ) {

    #if SAVE_INFO_INTERVAL_MS > 0
      next_save_ms = ms + SAVE_INFO_INTERVAL_MS;
    #endif

    // Set Head and Foot to matching non-zero values
    if (!++info.valid_head) ++info.valid_head; // non-zero in sequence
    //if (!IS_SD_PRINTING()) info.valid_head = 0;
    info.valid_foot = info.valid_head;

    // Machine state
    info.current_position = current_position;
    info.feedrate = uint16_t(MMS_TO_MMM(feedrate_mm_s));
    info.zraise = zraise;
    info.flag.raised = raised;                      // Was Z raised before power-off?

    TERN_(GCODE_REPEAT_MARKERS, info.stored_repeat = repeat);
    TERN_(HAS_HOME_OFFSET, info.home_offset = home_offset);
    TERN_(HAS_POSITION_SHIFT, info.position_shift = position_shift);
    E_TERN_(info.active_extruder = active_extruder);

    #if HAS_FAN
      COPY(info.fan_speed, thermalManager.fan_speed);
    #endif

    // Elapsed CNC job time
    info.print_job_elapsed = print_job_timer.duration();

    // Relative axis modes
    info.axis_relative = gcode.axis_relative;

    // Misc. mvCNC flags
    info.flag.dryrun               = !!(mvcnc_debug_flags & MVCNC_DEBUG_DRYRUN);
    info.flag.allow_cold_extrusion = TERN0(PREVENT_COLD_EXTRUSION, thermalManager.allow_cold_extrude);

    write();
  }
}

#if PIN_EXISTS(POWER_LOSS)

  #if ENABLED(BACKUP_POWER_SUPPLY)

    void CNCJobRecovery::retract_and_lift(const_float_t zraise) {
      #if POWER_LOSS_RETRACT_LEN || POWER_LOSS_ZRAISE

        gcode.set_relative_mode(true);  // Use relative coordinates

        #if POWER_LOSS_RETRACT_LEN
          // Retract filament now
          gcode.process_subcommands_now(F("G1 F3000 E-" STRINGIFY(POWER_LOSS_RETRACT_LEN)));
        #endif

        #if POWER_LOSS_ZRAISE
          // Raise the Z axis now
          if (zraise) {
            char cmd[20], str_1[16];
            sprintf_P(cmd, PSTR("G0Z%s"), dtostrf(zraise, 1, 3, str_1));
            gcode.process_subcommands_now(cmd);
          }
        #else
          UNUSED(zraise);
        #endif

        //gcode.axis_relative = info.axis_relative;
        planner.synchronize();
      #endif
    }

  #endif

  /**
   * An outage was detected by a sensor pin.
   *  - If not SD printing, let the machine turn off on its own with no "KILL" screen
   *  - Disable all heaters first to save energy
   *  - Save the recovery data for the current instant
   *  - If backup power is available Retract E and Raise Z
   *  - Go to the KILL screen
   */
  void CNCJobRecovery::_outage() {
    #if ENABLED(BACKUP_POWER_SUPPLY)
      static bool lock = false;
      if (lock) return; // No re-entrance from idle() during retract_and_lift()
      lock = true;
    #endif

    #if POWER_LOSS_ZRAISE
      // Get the limited Z-raise to do now or on resume
      const float zraise = _MAX(0, _MIN(current_position.z + POWER_LOSS_ZRAISE, Z_MAX_POS - 1) - current_position.z);
    #else
      constexpr float zraise = 0;
    #endif

    // Save the current position, distance that Z was (or should be) raised,
    // and a flag whether the raise was already done here.
    if (IS_SD_PRINTING()) save(true, zraise, ENABLED(BACKUP_POWER_SUPPLY));

    // Disable all heaters to reduce power loss
    thermalManager.disable_all_heaters();

    #if ENABLED(BACKUP_POWER_SUPPLY)
      // Do a hard-stop of the steppers (with possibly a loud thud)
      quickstop_stepper();
      // With backup power a retract and raise can be done now
      retract_and_lift(zraise);
    #endif

    kill(GET_TEXT_F(MSG_OUTAGE_RECOVERY));
  }

#endif

/**
 * Save the recovery info the recovery file
 */
void CNCJobRecovery::write() {

  debug(F("Write"));

  open(false);
  file.seekSet(0);
  const int16_t ret = file.write(&info, sizeof(info));
  if (ret == -1) DEBUG_ECHOLNPGM("Power-loss file write failed.");
  if (!file.close()) DEBUG_ECHOLNPGM("Power-loss file close failed.");
}

/**
 * Resume the saved CNC job
 */
void CNCJobRecovery::resume() {

  char cmd[MAX_CMD_SIZE+16], str_1[16], str_2[16];

  const uint32_t resume_sdpos = info.sdpos; // Get here before the stepper ISR overwrites it

  // Apply the dry-run flag if enabled
  if (info.flag.dryrun) mvcnc_debug_flags |= MVCNC_DEBUG_DRYRUN;

  // Interpret the saved Z according to flags
  const float z_print = info.current_position.z,
              z_raised = z_print + info.zraise;

  //
  // Home the axes that can safely be homed, and
  // establish the current position as best we can.
  //

  gcode.process_subcommands_now(F("G92.9E0")); // Reset E to 0

  #if Z_HOME_TO_MAX

    float z_now = z_raised;

    // If Z homing goes to max then just move back to the "raised" position
    sprintf_P(cmd, PSTR(
            "G28R0\n"     // Home all axes (no raise)
            "G1Z%sF1200"  // Move Z down to (raised) height
          ), dtostrf(z_now, 1, 3, str_1));
    gcode.process_subcommands_now(cmd);

  #elif DISABLED(BELTPRINTER)

    #if ENABLED(POWER_LOSS_RECOVER_ZHOME) && defined(POWER_LOSS_ZHOME_POS)
      #define HOMING_Z_DOWN 1
    #else
      #define HOME_XY_ONLY 1
    #endif

    float z_now = info.flag.raised ? z_raised : z_print;

    // Reset E to 0 and set Z to the real position
    #if HOME_XY_ONLY
      sprintf_P(cmd, PSTR("G92.9Z%s"), dtostrf(z_now, 1, 3, str_1));
      gcode.process_subcommands_now(cmd);
    #endif

    // Does Z need to be raised now? It should be raised before homing XY.
    if (z_raised > z_now) {
      z_now = z_raised;
      sprintf_P(cmd, PSTR("G1Z%sF600"), dtostrf(z_now, 1, 3, str_1));
      gcode.process_subcommands_now(cmd);
    }

    // Home XY with no Z raise, and also home Z here if Z isn't homing down below.
    gcode.process_subcommands_now(F("G28R0" TERN_(HOME_XY_ONLY, "XY"))); // No raise during G28

  #endif

  #if HOMING_Z_DOWN
    // Move to a safe XY position and home Z while avoiding the print.
    constexpr xy_pos_t p = POWER_LOSS_ZHOME_POS;
    sprintf_P(cmd, PSTR("G1X%sY%sF1000\nG28Z"), dtostrf(p.x, 1, 3, str_1), dtostrf(p.y, 1, 3, str_2));
    gcode.process_subcommands_now(cmd);
  #endif

  // Mark all axes as having been homed (no effect on current_position)
  set_all_homed();

  #if ENABLED(POWER_LOSS_RECOVER_ZHOME)
    // Z was homed down to the bed, so move up to the raised height.
    z_now = z_raised;
    sprintf_P(cmd, PSTR("G1Z%sF600"), dtostrf(z_now, 1, 3, str_1));
    gcode.process_subcommands_now(cmd);
  #endif

  // Restore the previously active tool (with no_move)
  #if HAS_MULTI_EXTRUDER || TOOL_CHANGE_SUPPORT
    sprintf_P(cmd, PSTR("T%i S"), info.active_extruder);
    gcode.process_subcommands_now(cmd);
  #endif

  // Restore print cooling fan speeds
  #if HAS_FAN
    FANS_LOOP(i) {
      const int f = info.fan_speed[i];
      if (f) {
        sprintf_P(cmd, PSTR("M106P%iS%i"), i, f);
        gcode.process_subcommands_now(cmd);
      }
    }
  #endif

  // Move back over to the saved XY
  sprintf_P(cmd, PSTR("G1X%sY%sF3000"),
    dtostrf(info.current_position.x, 1, 3, str_1),
    dtostrf(info.current_position.y, 1, 3, str_2)
  );
  gcode.process_subcommands_now(cmd);

  // Move back down to the saved Z for printing
  sprintf_P(cmd, PSTR("G1Z%sF600"), dtostrf(z_print, 1, 3, str_1));
  gcode.process_subcommands_now(cmd);

  // Restore the feedrate
  sprintf_P(cmd, PSTR("G1F%d"), info.feedrate);
  gcode.process_subcommands_now(cmd);

  // Restore E position with G92.9
  sprintf_P(cmd, PSTR("G92.9E%s"), dtostrf(info.current_position.e, 1, 3, str_1));
  gcode.process_subcommands_now(cmd);

  TERN_(GCODE_REPEAT_MARKERS, repeat = info.stored_repeat);
  TERN_(HAS_HOME_OFFSET, home_offset = info.home_offset);
  TERN_(HAS_POSITION_SHIFT, position_shift = info.position_shift);
  #if HAS_HOME_OFFSET || HAS_POSITION_SHIFT
    LOOP_LINEAR_AXES(i) update_workspace_offset((AxisEnum)i);
  #endif

  // Relative axis modes
  gcode.axis_relative = info.axis_relative;

  #if ENABLED(DEBUG_POWER_LOSS_RECOVERY)
    const uint8_t old_flags = mvcnc_debug_flags;
    mvcnc_debug_flags |= MVCNC_DEBUG_ECHO;
  #endif

  // Continue to apply PLR when a file is resumed!
  enable(true);

  // Resume the SD file from the last position
  char *fn = info.sd_filename;
  sprintf_P(cmd, M23_STR, fn);
  gcode.process_subcommands_now(cmd);
  sprintf_P(cmd, PSTR("M24S%ldT%ld"), resume_sdpos, info.print_job_elapsed);
  gcode.process_subcommands_now(cmd);

  TERN_(DEBUG_POWER_LOSS_RECOVERY, mvcnc_debug_flags = old_flags);
}

#if ENABLED(DEBUG_POWER_LOSS_RECOVERY)

  void CNCJobRecovery::debug(FSTR_P const prefix) {
    DEBUG_ECHOF(prefix);
    DEBUG_ECHOLNPGM(" Job Recovery Info...\nvalid_head:", info.valid_head, " valid_foot:", info.valid_foot);
    if (info.valid_head) {
      if (info.valid_head == info.valid_foot) {
        DEBUG_ECHOPGM("current_position: ");
        LOOP_LOGICAL_AXES(i) {
          if (i) DEBUG_CHAR(',');
          DEBUG_DECIMAL(info.current_position[i]);
        }
        DEBUG_EOL();

        DEBUG_ECHOLNPGM("feedrate: ", info.feedrate);

        DEBUG_ECHOLNPGM("zraise: ", info.zraise, " ", info.flag.raised ? "(before)" : "");

        #if ENABLED(GCODE_REPEAT_MARKERS)
          DEBUG_ECHOLNPGM("repeat index: ", info.stored_repeat.index);
          LOOP_L_N(i, info.stored_repeat.index)
            DEBUG_ECHOLNPGM("..... sdpos: ", info.stored_repeat.marker.sdpos, " count: ", info.stored_repeat.marker.counter);
        #endif

        #if HAS_HOME_OFFSET
          DEBUG_ECHOPGM("home_offset: ");
          LOOP_LINEAR_AXES(i) {
            if (i) DEBUG_CHAR(',');
            DEBUG_DECIMAL(info.home_offset[i]);
          }
          DEBUG_EOL();
        #endif

        #if HAS_POSITION_SHIFT
          DEBUG_ECHOPGM("position_shift: ");
          LOOP_LINEAR_AXES(i) {
            if (i) DEBUG_CHAR(',');
            DEBUG_DECIMAL(info.position_shift[i]);
          }
          DEBUG_EOL();
        #endif

        #if HAS_MULTI_EXTRUDER
          DEBUG_ECHOLNPGM("active_extruder: ", info.active_extruder);
        #endif

        #if HAS_FAN
          DEBUG_ECHOPGM("fan_speed: ");
          FANS_LOOP(i) {
            DEBUG_ECHO(info.fan_speed[i]);
            if (i < FAN_COUNT - 1) DEBUG_CHAR(',');
          }
          DEBUG_EOL();
        #endif

        DEBUG_ECHOLNPGM("sd_filename: ", info.sd_filename);
        DEBUG_ECHOLNPGM("sdpos: ", info.sdpos);
        DEBUG_ECHOLNPGM("print_job_elapsed: ", info.print_job_elapsed);

        DEBUG_ECHOPGM("axis_relative:");
        if (TEST(info.axis_relative, REL_X)) DEBUG_ECHOPGM(" REL_X");
        if (TEST(info.axis_relative, REL_Y)) DEBUG_ECHOPGM(" REL_Y");
        if (TEST(info.axis_relative, REL_Z)) DEBUG_ECHOPGM(" REL_Z");
        DEBUG_EOL();

        DEBUG_ECHOLNPGM("flag.dryrun: ", AS_DIGIT(info.flag.dryrun));
        DEBUG_ECHOLNPGM("flag.allow_cold_extrusion: ", AS_DIGIT(info.flag.allow_cold_extrusion));
        DEBUG_ECHOLNPGM("flag.volumetric_enabled: ", AS_DIGIT(info.flag.volumetric_enabled));
      }
      else
        DEBUG_ECHOLNPGM("INVALID DATA");
    }
    DEBUG_ECHOLNPGM("---");
  }

#endif // DEBUG_POWER_LOSS_RECOVERY

#endif // POWER_LOSS_RECOVERY
