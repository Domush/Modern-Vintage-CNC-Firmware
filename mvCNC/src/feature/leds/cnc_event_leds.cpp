/**
 * Modern Vintage CNC Firmware
 */

/**
 * feature/leds/cnc_event_leds.cpp - LED color changing based on cnc status
 */

#include "../../inc/mvCNCConfigPre.h"

#if ENABLED(CNC_EVENT_LEDS)

  #include "cnc_event_leds.h"

CNCEventLEDs cncEventLEDs;

  #if HAS_LEDS_OFF_FLAG
bool CNCEventLEDs::leds_off_after_job;  // = false
  #endif

  #if HAS_TEMP_HOTEND || HAS_HEATED_BED

uint8_t CNCEventLEDs::old_intensity = 0;

inline uint8_t pel_intensity(const celsius_t start, const celsius_t current, const celsius_t target) {
  if (start == target) return 255;
  return (uint8_t)map(constrain(current, start, target), start, target, 0, 255);
}

inline void pel_set_rgb(const uint8_t r, const uint8_t g, const uint8_t b OPTARG(HAS_WHITE_LED, const uint8_t w = 0)) {
  leds.set_color(
      LEDColor(r, g, b OPTARG(HAS_WHITE_LED, w) OPTARG(NEOPIXEL_LED, neo.brightness()))
          OPTARG(NEOPIXEL_IS_SEQUENTIAL, true));
}

  #endif

  #if HAS_TEMP_HOTEND

void CNCEventLEDs::onHotendHeating(const celsius_t start, const celsius_t current, const celsius_t target) {
  const uint8_t blue = pel_intensity(start, current, target);
  if (blue != old_intensity) {
    old_intensity = blue;
    pel_set_rgb(255, 0, 255 - blue);
  }
}

  #endif

  #if HAS_HEATED_BED

void CNCEventLEDs::onBedHeating(const celsius_t start, const celsius_t current, const celsius_t target) {
  const uint8_t red = pel_intensity(start, current, target);
  if (red != old_intensity) {
    old_intensity = red;
    pel_set_rgb(red, 0, 255);
  }
}

  #endif

  #if HAS_HEATED_CHAMBER

void CNCEventLEDs::onChamberHeating(const celsius_t start, const celsius_t current, const celsius_t target) {
  const uint8_t green = pel_intensity(start, current, target);
  if (green != old_intensity) {
    old_intensity = green;
    pel_set_rgb(255, green, 255);
  }
}

  #endif

#endif  // CNC_EVENT_LEDS
