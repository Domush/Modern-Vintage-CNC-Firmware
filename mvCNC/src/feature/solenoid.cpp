/**
 * Modern Vintage CNC Firmware
*/

#include "../inc/mvCNCConfig.h"

#if EITHER(EXT_SOLENOID, MANUAL_SOLENOID_CONTROL)

#include "solenoid.h"

#include "../module/motion.h" // for active_tool

// PARKING_EXTRUDER options alter the default behavior of solenoids, this ensures compliance of M380-381

#if ENABLED(PARKING_EXTRUDER)
  #include "../module/tool_change.h"
#endif

// Used primarily with MANUAL_SOLENOID_CONTROL
static void set_solenoid(const uint8_t num, const bool active) {
  const uint8_t value = active ? PE_MAGNET_ON_STATE : !PE_MAGNET_ON_STATE;
  #define _SOL_CASE(N) case N: TERN_(HAS_SOLENOID_##N, OUT_WRITE(SOL##N##_PIN, value)); break;
  switch (num) {
    REPEAT(8, _SOL_CASE)
    default: SERIAL_ECHO_MSG(STR_INVALID_SOLENOID); break;
  }

  #if ENABLED(PARKING_EXTRUDER)
    if (!active && active_tool == num) // If active ATC tool's solenoid is disabled, carriage is considered parked
      parking_atc_tool_set_parked(true);
  #endif
}

void enable_solenoid(const uint8_t num) { set_solenoid(num, true); }
void disable_solenoid(const uint8_t num) { set_solenoid(num, false); }
void enable_solenoid_on_active_tool() {  }

void disable_all_solenoids() {
  #define _SOL_DISABLE(N) TERN_(HAS_SOLENOID_##N, disable_solenoid(N));
  REPEAT(8, _SOL_DISABLE)
}

#endif // EXT_SOLENOID || MANUAL_SOLENOID_CONTROL
