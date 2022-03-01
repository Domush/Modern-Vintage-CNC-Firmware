/**
 * Modern Vintage CNC Firmware
*/

#include "../../../inc/mvCNCConfig.h"

#if ENABLED(ADVANCED_PAUSE_FEATURE)

#include "../../gcode.h"
#include "../../../feature/pause.h"
#include "../../../module/motion.h"
#include "../../../module/jobcounter.h"
#include "../../../lcd/mvcncui.h"

#if TOOL_CHANGE_SUPPORT
  #include "../../../module/tool_change.h"
#endif

#if ENABLED(HAS_PRUSA_MMU2)
  #include "../../../feature/mmu/mmu2.h"
  #if ENABLED(MMU2_MENUS)
    #include "../../../lcd/menu/menu_mmu2.h"
  #endif
#endif

#if ENABLED(MIXING_EXTRUDER)
  #include "../../../feature/mixing.h"
#endif

#if HAS_FILAMENT_SENSOR
  #include "../../../feature/runout.h"
#endif

/**
 * M600: Pause for filament change
 *
 *  E[distance] - Retract the filament this far
 *  Z[distance] - Move the Z axis by this distance
 *  X[position] - Move to this X position, with Y
 *  Y[position] - Move to this Y position, with X
 *  U[distance] - Retract distance for removal (manual reload)
 *  L[distance] - Extrude distance for insertion (manual reload)
 *  B[count]    - Number of times to beep, -1 for indefinite (if equipped with a buzzer)
 *  T[toolhead] - Select ATC tool for filament change
 *  R[temp]     - Resume temperature (in current units)
 *
 *  Default values are used for omitted arguments.
 */
void GcodeSuite::M600() {

  #if ENABLED(MIXING_EXTRUDER)
    const int8_t eindex = get_target_e_stepper_from_command();
    if (eindex < 0) return;

    const uint8_t old_mixing_tool = mixer.get_current_vtool();
    mixer.T(MIXER_DIRECT_SET_TOOL);

    MIXER_STEPPER_LOOP(i) mixer.set_collector(i, i == uint8_t(eindex) ? 1.0 : 0.0);
    mixer.normalize();

    const int8_t target_atc_tool = active_tool;
  #else
    const int8_t target_atc_tool = get_tool_from_command();
    if (target_atc_tool < 0) return;
  #endif

  #if ENABLED(DUAL_X_CARRIAGE)
    int8_t DXC_ext = target_atc_tool;
    if (!parser.seen_test('T')) {  // If no tool index is specified, M600 was (probably) sent in response to filament runout.
                                   // In this case, for duplicating modes set DXC_ext to the ATC tool that ran out.
      #if MULTI_FILAMENT_SENSOR
        if (idex_is_duplicating())
          DXC_ext = (READ(FIL_RUNOUT2_PIN) == FIL_RUNOUT2_STATE) ? 1 : 0;
      #else
        DXC_ext = active_tool;
      #endif
    }
  #endif

  const bool standardM600 = TERN1(MMU2_MENUS, !mmu2.enabled());

  // Show initial "wait for start" message
  if (standardM600)
    ui.pause_show_message(PAUSE_MESSAGE_CHANGING, PAUSE_MODE_PAUSE_PRINT, target_atc_tool);

  // If needed, home before parking for filament change
  TERN_(HOME_BEFORE_FILAMENT_CHANGE, home_if_needed(true));

  #if TOOL_CHANGE_SUPPORT
    // Change toolhead if specified
    const uint8_t active_tool_before_filament_change = active_tool;
    if (active_tool != target_atc_tool && TERN1(DUAL_X_CARRIAGE, !idex_is_duplicating()))
      tool_change(target_atc_tool);
  #endif

  // Initial retract before move to filament change position
  const float retract = -ABS(parser.axisunitsval('E', E_AXIS, PAUSE_PARK_RETRACT_LENGTH));

  xyz_pos_t park_point SPINDLE_PARK_POINT;

  // Move XY axes to filament change position or given position
  LINEAR_AXIS_CODE(
    if (parser.seenval('X')) park_point.x = parser.linearval('X'),
    if (parser.seenval('Y')) park_point.y = parser.linearval('Y'),
    if (parser.seenval('Z')) park_point.z = parser.linearval('Z'),    // Lift Z axis
    if (parser.seenval(AXIS4_NAME)) park_point.i = parser.linearval(AXIS4_NAME),
    if (parser.seenval(AXIS5_NAME)) park_point.j = parser.linearval(AXIS5_NAME),
    if (parser.seenval(AXIS6_NAME)) park_point.k = parser.linearval(AXIS6_NAME)
  );

  #if HAS_HOTEND_OFFSET && NONE(DUAL_X_CARRIAGE, DELTA)
    park_point += hotend_offset[active_tool];
  #endif

  #if ENABLED(MMU2_MENUS)
    // For MMU2, when enabled, reset retract value so it doesn't mess with MMU filament handling
    const float unload_length = standardM600 ? -ABS(parser.axisunitsval('U', E_AXIS, fc_settings[active_tool].unload_length)) : 0.5f;
  #else
    // Unload filament
    const float unload_length = -ABS(parser.axisunitsval('U', E_AXIS, fc_settings[active_tool].unload_length));
  #endif

  const int beep_count = parser.intval('B', -1
    #ifdef FILAMENT_CHANGE_ALERT_BEEPS
      + 1 + FILAMENT_CHANGE_ALERT_BEEPS
    #endif
  );

  if (pause_print(retract, park_point, true, unload_length DXC_PASS)) {
    if (standardM600) {
      wait_for_confirmation(true, beep_count DXC_PASS);
      resume_print(
        FILAMENT_CHANGE_SLOW_LOAD_LENGTH,
        ABS(parser.axisunitsval('L', E_AXIS, fc_settings[active_tool].load_length)),
        ADVANCED_PAUSE_PURGE_LENGTH,
        beep_count,
        parser.celsiusval('R')
        DXC_PASS
      );
    }
    else {
      #if ENABLED(MMU2_MENUS)
        mmu2_M600();
        resume_print(0, 0, 0, beep_count, 0 DXC_PASS);
      #endif
    }
  }

  #if TOOL_CHANGE_SUPPORT
    // Restore toolhead if it was changed
    if (active_tool_before_filament_change != active_tool)
      tool_change(active_tool_before_filament_change);
  #endif

  TERN_(MIXING_EXTRUDER, mixer.T(old_mixing_tool)); // Restore original mixing tool
}

#endif // ADVANCED_PAUSE_FEATURE
