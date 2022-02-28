/**
 * Modern Vintage CNC Firmware
*/

#include "../../inc/mvCNCConfig.h"

#if HAS_FAN

#include "../gcode.h"
#include "../../module/motion.h"
#include "../../module/fan_control.h"

#if ENABLED(LASER_SYNCHRONOUS_M106_M107)
  #include "../../module/planner.h"
#endif

#if HAS_PREHEAT
  #include "../../lcd/mvcncui.h"
#endif

#if ENABLED(SINGLENOZZLE)
  #define _ALT_P active_extruder
  #define _CNT_P EXTRUDERS
#else
  #define _ALT_P _MIN(active_extruder, FAN_COUNT - 1)
  #define _CNT_P FAN_COUNT
#endif

/**
 * M106: Set Fan Speed
 *
 *  I<index> Material Preset index (if material presets are defined)
 *  S<int>   Speed between 0-255
 *  P<index> Fan index, if more than one fan
 *
 * With EXTRA_FAN_SPEED enabled:
 *
 *  T<int>   Restore/Use/Set Temporary Speed:
 *           1     = Restore previous speed after T2
 *           2     = Use temporary speed set with T3-255
 *           3-255 = Set the speed for use with T2
 */
void GcodeSuite::M106() {
  const uint8_t pfan = parser.byteval('P', _ALT_P);
  if (pfan >= _CNT_P) return;
  #if REDUNDANT_PART_COOLING_FAN
    if (pfan == REDUNDANT_PART_COOLING_FAN) return;
  #endif

  #if ENABLED(EXTRA_FAN_SPEED)
    const uint16_t t = parser.intval('T');
    if (t > 0) return fanManager.fanSpeedOverride(pfan, t);
  #endif

  const uint16_t dspeed = parser.seen_test('A') ? fanManager.fan_speed[active_extruder] : 255;

  uint16_t speed = dspeed;

  // Accept 'I' if temperature presets are defined
  #if HAS_PREHEAT
    const bool got_preset = parser.seenval('I');
    if (got_preset) speed = ui.material_preset[_MIN(parser.value_byte(), PREHEAT_COUNT - 1)].fan_speed;
  #else
    constexpr bool got_preset = false;
  #endif

  if (!got_preset && parser.seenval('S'))
    speed = parser.value_ushort();

  TERN_(FOAMCUTTER_XYUV, speed *= 2.55); // Get command in % of max heat

  // Set speed, with constraint
  fanManager.fanSpeedSet(pfan, speed);

  TERN_(LASER_SYNCHRONOUS_M106_M107, planner.buffer_sync_block(BLOCK_FLAG_SYNC_FANS));

  if (TERN0(DUAL_X_CARRIAGE, idex_is_duplicating()))  // pfan == 0 when duplicating
    fanManager.fanSpeedSet(1 - pfan, speed);
}

/**
 * M107: Fan Off
 */
void GcodeSuite::M107() {
  const uint8_t pfan = parser.byteval('P', _ALT_P);
  if (pfan >= _CNT_P) return;
  #if REDUNDANT_PART_COOLING_FAN
    if (pfan == REDUNDANT_PART_COOLING_FAN) return;
  #endif

  fanManager.fanSpeedSet(pfan, 0);

  if (TERN0(DUAL_X_CARRIAGE, idex_is_duplicating()))  // pfan == 0 when duplicating
    fanManager.fanSpeedSet(1 - pfan, 0);

  TERN_(LASER_SYNCHRONOUS_M106_M107, planner.buffer_sync_block(BLOCK_FLAG_SYNC_FANS));
}

#endif // HAS_FAN
