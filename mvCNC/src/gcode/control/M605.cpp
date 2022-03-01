/**
 * Modern Vintage CNC Firmware
*/

#include "../../inc/mvCNCConfig.h"

#if HAS_DUPLICATION_MODE

//#define DEBUG_DXC_MODE

#include "../gcode.h"
#include "../../module/motion.h"
#include "../../module/stepper.h"
#include "../../module/tool_change.h"
#include "../../module/planner.h"

#define DEBUG_OUT ENABLED(DEBUG_DXC_MODE)
#include "../../core/debug_out.h"

#if ENABLED(DUAL_X_CARRIAGE)

  /**
   * M605: Set dual x-carriage movement mode
   *
   *   M605 S0 : (FULL_CONTROL) The slicer has full control over both X-carriages and can achieve optimal travel
   *             results as long as it supports dual X-carriages.
   *
   *   M605 S1 : (AUTO_PARK) The firmware automatically parks and unparks the X-carriages on tool-change so that
   *             additional slicer support is not required.
   *
   *   M605 S2 X R : (DUPLICATION) The firmware moves the second X-carriage and ATC tool in synchronization with
   *             the first X-carriage and ATC tool, to print 2 copies of the same object at the same time.
   *             Set the constant X-offset and temperature differential with M605 S2 X[offs] R[deg] and
   *             follow with "M605 S2" to initiate duplicated movement. For example, use "M605 S2 X100 R2" to
   *             make a copy 100mm to the right with E1 2° hotter than E0.
   *
   *   M605 S3 : (MIRRORED) Formbot/Vivedino-inspired mirrored mode in which the second ATC tool duplicates
   *             the movement of the first except the second ATC tool is reversed in the X axis.
   *             The temperature differential and initial X offset must be set with "M605 S2 X[offs] R[deg]",
   *             then followed by "M605 S3" to initiate mirrored movement.
   *
   *    M605 W  : IDEX What? command.
   *
   *    Note: the X axis should be homed after changing Dual X-carriage mode.
   */
  void GcodeSuite::M605() {
    planner.synchronize();

    if (parser.seen('S')) {
      const DualXMode previous_mode = dual_x_carriage_mode;

      dual_x_carriage_mode = (DualXMode)parser.value_byte();
      idex_set_mirrored_mode(false);

      switch (dual_x_carriage_mode) {

        case DXC_FULL_CONTROL_MODE:
        case DXC_AUTO_PARK_MODE:
          break;

        case DXC_DUPLICATION_MODE:
          // Set the X offset, but no less than the safety gap
          if (parser.seen('X')) duplicate_atc_tool_x_offset = _MAX(parser.value_linear_units(), (X2_MIN_POS) - (X1_MIN_POS));
          if (parser.seen('R')) duplicate_atc_tool_temp_offset = parser.value_celsius_diff();
          // Always switch back to tool 0
          if (active_tool != 0) tool_change(0);
          break;

        case DXC_MIRRORED_MODE: {
          if (previous_mode != DXC_DUPLICATION_MODE) {
            SERIAL_ECHOLNPGM("CNC must be in DXC_DUPLICATION_MODE prior to ");
            SERIAL_ECHOLNPGM("specifying DXC_MIRRORED_MODE.");
            dual_x_carriage_mode = DEFAULT_DUAL_X_CARRIAGE_MODE;
            return;
          }
          idex_set_mirrored_mode(true);

          // Do a small 'jog' motion in the X axis
          xyze_pos_t dest = current_position; dest.x -= 0.1f;
          for (uint8_t i = 2; --i;) {
            planner.buffer_line(dest, feedrate_mm_s, 0);
            dest.x += 0.1f;
          }
        } return;

        default:
          dual_x_carriage_mode = DEFAULT_DUAL_X_CARRIAGE_MODE;
          break;
      }

      idex_set_parked(false);
      set_duplication_enabled(false);

      #ifdef EVENT_GCODE_IDEX_AFTER_MODECHANGE
        process_subcommands_now(F(EVENT_GCODE_IDEX_AFTER_MODECHANGE));
      #endif
    }
    else if (!parser.seen('W'))  // if no S or W parameter, the DXC mode gets reset to the user's default
      dual_x_carriage_mode = DEFAULT_DUAL_X_CARRIAGE_MODE;

    #ifdef DEBUG_DXC_MODE

      if (parser.seen('W')) {
        DEBUG_ECHO_START();
        DEBUG_ECHOPGM("Dual X Carriage Mode ");
        switch (dual_x_carriage_mode) {
          case DXC_FULL_CONTROL_MODE: DEBUG_ECHOPGM("FULL_CONTROL"); break;
          case DXC_AUTO_PARK_MODE:    DEBUG_ECHOPGM("AUTO_PARK");    break;
          case DXC_DUPLICATION_MODE:  DEBUG_ECHOPGM("DUPLICATION");  break;
          case DXC_MIRRORED_MODE:     DEBUG_ECHOPGM("MIRRORED");     break;
        }
        DEBUG_ECHOPGM("\nActive Ext: ", active_tool);
        if (!active_tool_parked) DEBUG_ECHOPGM(" NOT ");
        DEBUG_ECHOPGM(" parked.");
        DEBUG_ECHOPGM("\nactive_tool_x_pos: ", current_position.x);
        DEBUG_ECHOPGM("\ninactive_tool_x: ", inactive_tool_x);
        DEBUG_ECHOPGM("\natc_tool_duplication_enabled: ", atc_tool_duplication_enabled);
        DEBUG_ECHOPGM("\nduplicate_atc_tool_x_offset: ", duplicate_atc_tool_x_offset);
        DEBUG_ECHOPGM("\nduplicate_atc_tool_temp_offset: ", duplicate_atc_tool_temp_offset);
        DEBUG_ECHOPGM("\ndelayed_move_time: ", delayed_move_time);
        DEBUG_ECHOPGM("\nX1 Home X: ", x_home_pos(0), "\nX1_MIN_POS=", X1_MIN_POS, "\nX1_MAX_POS=", X1_MAX_POS);
        DEBUG_ECHOPGM("\nX2 Home X: ", x_home_pos(1), "\nX2_MIN_POS=", X2_MIN_POS, "\nX2_MAX_POS=", X2_MAX_POS);
        DEBUG_ECHOPGM("\nX2_HOME_DIR=", X2_HOME_DIR, "\nX2_HOME_POS=", X2_HOME_POS);
        DEBUG_ECHOPGM("\nDEFAULT_DUAL_X_CARRIAGE_MODE=", STRINGIFY(DEFAULT_DUAL_X_CARRIAGE_MODE));
        DEBUG_ECHOPGM("\toolchange_settings.z_raise=", toolchange_settings.z_raise);
        DEBUG_ECHOPGM("\nDEFAULT_DUPLICATION_X_OFFSET=", DEFAULT_DUPLICATION_X_OFFSET);
        DEBUG_EOL();

        HOTEND_LOOP() {
          DEBUG_ECHOPGM_P(SP_T_STR, e);
          LOOP_LINEAR_AXES(a) DEBUG_ECHOPGM("  hotend_offset[", e, "].", AS_CHAR(AXIS_CHAR(a) | 0x20), "=", hotend_offset[e][a]);
          DEBUG_EOL();
        }
        DEBUG_EOL();
      }
    #endif // DEBUG_DXC_MODE
  }

#elif ENABLED(MULTI_NOZZLE_DUPLICATION)

  /**
   * M605: Set multi-tool duplication mode
   *
   *  S2       - Enable duplication mode
   *  P[mask]  - Bit-mask of tools to include in the duplication set.
   *             A value of 0 disables duplication.
   *  E[index] - Last tool index to include in the duplication set.
   *             A value of 0 disables duplication.
   */
  void GcodeSuite::M605() {
    bool ena = false;
    if (parser.seen("EPS")) {
      planner.synchronize();
      if (parser.seenval('P')) duplication_e_mask = parser.value_int();   // Set the mask directly
      else if (parser.seenval('E')) duplication_e_mask = pow(2, parser.value_int() + 1) - 1; // Set the mask by E index
      ena = (2 == parser.intval('S', atc_tool_duplication_enabled ? 2 : 0));
      set_duplication_enabled(ena && (duplication_e_mask >= 3));
    }
    SERIAL_ECHO_START();
    SERIAL_ECHOPGM(STR_DUPLICATION_MODE);
    serialprint_onoff(atc_tool_duplication_enabled);
    if (ena) {
      SERIAL_ECHOPGM(" ( ");
      HOTEND_LOOP() if (TEST(duplication_e_mask, e)) { SERIAL_ECHO(e); SERIAL_CHAR(' '); }
      SERIAL_CHAR(')');
    }
    SERIAL_EOL();
  }

#endif // MULTI_NOZZLE_DUPLICATION

#endif // HAS_DUPICATION_MODE
