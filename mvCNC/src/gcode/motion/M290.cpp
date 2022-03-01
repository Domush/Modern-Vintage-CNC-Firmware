/**
 * Modern Vintage CNC Firmware
*/

#include "../../inc/mvCNCConfig.h"

#if ENABLED(BABYSTEPPING)

#include "../gcode.h"
#include "../../feature/babystep.h"
#include "../../module/probe.h"
#include "../../module/planner.h"

#if ENABLED(BABYSTEP_ZPROBE_OFFSET)
  #include "../../core/serial.h"
#endif

#if ENABLED(MESH_BED_LEVELING)
  #include "../../feature/bedlevel/bedlevel.h"
#endif

#if ENABLED(BABYSTEP_ZPROBE_OFFSET)

  FORCE_INLINE void mod_probe_offset(const_float_t offs) {
    if (TERN1(BABYSTEP_HOTEND_Z_OFFSET, active_tool == 0)) {
      probe.offset.z += offs;
      SERIAL_ECHO_MSG(STR_PROBE_OFFSET " " STR_Z, probe.offset.z);
    }
    else {
      #if ENABLED(BABYSTEP_HOTEND_Z_OFFSET)
        hotend_offset[active_tool].z -= offs;
        SERIAL_ECHO_MSG(STR_PROBE_OFFSET STR_Z ": ", hotend_offset[active_tool].z);
      #endif
    }
  }

#endif

/**
 * M290: Babystepping
 *
 * Send 'R' or no parameters for a report.
 *
 *  X<linear> - Distance to step X
 *  Y<linear> - Distance to step Y
 *  Z<linear> - Distance to step Z
 *  S<linear> - Distance to step Z (alias for Z)
 *
 * With BABYSTEP_ZPROBE_OFFSET:
 *  P0 - Don't adjust the Z probe offset
 */
void GcodeSuite::M290() {
  #if ENABLED(BABYSTEP_XY)
    LOOP_LINEAR_AXES(a)
      if (parser.seenval(AXIS_CHAR(a)) || (a == Z_AXIS && parser.seenval('S'))) {
        const float offs = constrain(parser.value_axis_units((AxisEnum)a), -2, 2);
        babystep.add_mm((AxisEnum)a, offs);
        #if ENABLED(BABYSTEP_ZPROBE_OFFSET)
          if (a == Z_AXIS && parser.boolval('P', true)) mod_probe_offset(offs);
        #endif
      }
  #else
    if (parser.seenval('Z') || parser.seenval('S')) {
      const float offs = constrain(parser.value_axis_units(Z_AXIS), -2, 2);
      babystep.add_mm(Z_AXIS, offs);
      #if ENABLED(BABYSTEP_ZPROBE_OFFSET)
        if (parser.boolval('P', true)) mod_probe_offset(offs);
      #endif
    }
  #endif

  if (!parser.seen(LINEAR_AXIS_GANG("X", "Y", "Z", STR_I, STR_J, STR_K)) || parser.seen('R')) {
    SERIAL_ECHO_START();

    #if ENABLED(BABYSTEP_ZPROBE_OFFSET)
      SERIAL_ECHOLNPGM(STR_PROBE_OFFSET " " STR_Z, probe.offset.z);
    #endif

    #if ENABLED(BABYSTEP_HOTEND_Z_OFFSET)
    {
      SERIAL_ECHOLNPGM_P(
        PSTR("Hotend "), active_tool
        #if ENABLED(BABYSTEP_XY)
          , PSTR("Offset X"), hotend_offset[active_tool].x
          , SP_Y_STR, hotend_offset[active_tool].y
          , SP_Z_STR
        #else
          , PSTR("Offset Z")
        #endif
        , hotend_offset[active_tool].z
      );
    }
    #endif

    #if ENABLED(MESH_BED_LEVELING)
      SERIAL_ECHOLNPGM("MBL Adjust Z", mbl.z_offset);
    #endif

    #if ENABLED(BABYSTEP_DISPLAY_TOTAL)
    {
      SERIAL_ECHOLNPGM_P(
        #if ENABLED(BABYSTEP_XY)
            PSTR("Babystep X"), babystep.axis_total[X_AXIS]
          , SP_Y_STR, babystep.axis_total[Y_AXIS]
          , SP_Z_STR
        #else
          PSTR("Babystep Z")
        #endif
        , babystep.axis_total[BS_TOTAL_IND(Z_AXIS)]
      );
    }
    #endif
  }
}

#endif // BABYSTEPPING
