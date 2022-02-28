/**
 * Modern Vintage CNC Firmware
*/

/**
 * mvCNC RGB LED general support
 */

#include "../../inc/mvCNCConfig.h"

#if ENABLED(TEMP_STAT_LEDS)

#include "tempstat.h"
#include "../../module/fan_control.h"

void handle_status_leds() {
  static int8_t old_red = -1;  // Invalid value to force LED initialization
  static millis_t next_status_led_update_ms = 0;
  if (ELAPSED(millis(), next_status_led_update_ms)) {
    next_status_led_update_ms += 500; // Update every 0.5s
    celsius_t max_temp = TERN0(HAS_HEATED_BED, _MAX(thermalManager.degTargetBed(), thermalManager.wholeDegBed()));
    HOTEND_LOOP()
      max_temp = _MAX(max_temp, thermalManager.wholeDegHotend(e), thermalManager.degTargetHotend(e));
    const int8_t new_red = (max_temp > 55) ? HIGH : (max_temp < 54 || old_red < 0) ? LOW : old_red;
    if (new_red != old_red) {
      old_red = new_red;
      #if PIN_EXISTS(STAT_LED_RED)
        WRITE(STAT_LED_RED_PIN, new_red);
      #endif
      #if PIN_EXISTS(STAT_LED_BLUE)
        WRITE(STAT_LED_BLUE_PIN, !new_red);
      #endif
    }
  }
}

#endif // TEMP_STAT_LEDS
