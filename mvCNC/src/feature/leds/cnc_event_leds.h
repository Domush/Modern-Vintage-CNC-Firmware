/**
 * Modern Vintage CNC Firmware
 */
#pragma once

/**
 * feature/leds/cnc_event_leds.h - LED color changing based on cnc status
 */

#include "leds.h"
#include "../../inc/mvCNCConfig.h"

class CNCEventLEDs {
 private:
  static uint8_t old_intensity;

#if HAS_LEDS_OFF_FLAG
  static bool leds_off_after_job;
#endif

  static void set_done() { TERN(LED_COLOR_PRESETS, leds.set_default(), leds.set_off()); }

 public:
#if HAS_TEMP_HOTEND
  static LEDColor onHotendHeatingStart() {
    old_intensity = 0;
    return leds.get_color();
  }
  static void onHotendHeating(const celsius_t start, const celsius_t current, const celsius_t target);
#endif

#if HAS_HEATED_BED
  static LEDColor onBedHeatingStart() {
    old_intensity = 127;
    return leds.get_color();
  }
  static void onBedHeating(const celsius_t start, const celsius_t current, const celsius_t target);
#endif

#if HAS_HEATED_CHAMBER
  static LEDColor onChamberHeatingStart() {
    old_intensity = 127;
    return leds.get_color();
  }
  static void onChamberHeating(const celsius_t start, const celsius_t current, const celsius_t target);
#endif

#if HAS_TEMP_HOTEND || HAS_HEATED_BED || HAS_HEATED_CHAMBER
  static void onHeatingDone() { leds.set_white(); }
  static void onPidTuningDone(LEDColor c) { leds.set_color(c); }
#endif

#if ENABLED(SDSUPPORT)

  static void onPrintCompleted() {
    leds.set_green();
  #if HAS_LEDS_OFF_FLAG
    leds_off_after_job = true;
  #else
    safe_delay(2000);
    set_done();
  #endif
  }

  static void onResumeAfterWait() {
  #if HAS_LEDS_OFF_FLAG
    if (leds_off_after_job) {
      set_done();
      leds_off_after_job = false;
    }
  #endif
  }

#endif  // SDSUPPORT
};

extern CNCEventLEDs cncEventLEDs;
