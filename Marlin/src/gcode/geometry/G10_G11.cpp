/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "../../inc/MarlinConfig.h"

#if ENABLED(FWRETRACT)
  #include "../../feature/fwretract.h"
#endif

#if EITHER(FWRETRACT, CNC_COORDINATE_SYSTEMS)
  #include "../gcode.h"
  #include "../../module/motion.h"
  #include "../../module/stepper.h"

/**
 * G10 S#         - Retract filament according to settings of M207
 *     L2/L20 P# X# Y# Z#... - Sets the origin of the coordinate system, specified by the P parameter (1 to 9)
 *
 * Beta L2/L20 implementation based on https://github.com/MarlinFirmware/Marlin/issues/14734
 * and http://linuxcnc.org/docs/2.6/html/gcode/gcode.html#sec:G10-L1_
 *
 * *Supported CNC_COORDINATE_SYSTEMS parameters: L2 and L20
 * *Unsupported parameters: L1
 * !Beta - Use with caution.
 */
void GcodeSuite::G10() {
  #if ENABLED(FWRETRACT)
  fwretract.retract(true E_OPTARG(parser.boolval('S')));
  #endif
  #if ENABLED(CNC_COORDINATE_SYSTEMS)

  const int8_t target_system = parser.intval('P') - 1,
               offset_type   = parser.intval('L');

  if (target_system > 0 && target_system < MAX_COORDINATE_SYSTEMS) {
    const int8_t current_system              = active_coordinate_system; // Store current coord system
    const boolean target_system_is_different = current_system != target_system;
    if (target_system_is_different) select_coordinate_system(target_system); // Select New Coordinate System If Needed
    switch (offset_type) {
      case 1: // Sets the tool offset, as if the L parameter was not present
        SERIAL_ECHOLN("Error: L1 not supported, only L2 and L20");
        break;
      case 2: // Sets the origin of the coordinate system, specified by the P parameter (1 to 9), to the X, Y, Z... values
        LOOP_LOGICAL_AXES(i) {
          if (parser.seenval(axis_codes[i])) {
            const float l = parser.value_axis_units((AxisEnum)i),                                // Given axis coordinate value, converted to millimeters
                v         = TERN0(HAS_EXTRUDERS, i == E_AXIS) ? l : LOGICAL_TO_NATIVE(l, i) - l, // Axis position in NATIVE space (applying the existing offset)
                d         = v - current_position[i];                                             // How much is the current axis position altered by?
            if (!NEAR_ZERO(d)) {
              if (TERN1(HAS_EXTRUDERS, i != E_AXIS)) {
                position_shift[i] += d; // ...most axes offset the workspace...
                update_workspace_offset((AxisEnum)i);
              }
            }
          }
        }
        // Apply Workspace Offset to the active coordinate system
        coordinate_system[active_coordinate_system] = position_shift;
        break;
      case 20: // Sets the origin of the coordinate system, specified by the P parameter (1 to 9), to the current tool location
        LOOP_LOGICAL_AXES(i) {
          if (parser.seenval(axis_codes[i])) {
            const float l = parser.value_axis_units((AxisEnum)i),                            // Given axis coordinate value, converted to millimeters
                v         = TERN0(HAS_EXTRUDERS, i == E_AXIS) ? l : LOGICAL_TO_NATIVE(l, i), // Axis position in NATIVE space (applying the existing offset)
                d         = v - current_position[i];                                         // How much is the current axis position altered by?
            if (!NEAR_ZERO(d)) {
              if (TERN1(HAS_EXTRUDERS, i != E_AXIS)) {
                position_shift[i] += d; // ...most axes offset the workspace...
                update_workspace_offset((AxisEnum)i);
              }
            }
          }
        }
        // Apply Workspace Offset to the active coordinate system
        coordinate_system[active_coordinate_system] = position_shift;
        break;
    }
    if (target_system_is_different) select_coordinate_system(current_system); // Go Back To Original Coord System If Needed
    if (target_system_is_different) report_current_position();                // Report Current Position
  }
  #endif // ENABLED(CNC_COORDINATE_SYSTEMS)
}
#endif   // EITHER(FWRETRACT, CNC_COORDINATE_SYSTEMS)

#if ENABLED(FWRETRACT)
  /**
   * G11 - Recover filament according to settings of M208
   */
  void GcodeSuite::G11() { fwretract.retract(false); }
#endif
