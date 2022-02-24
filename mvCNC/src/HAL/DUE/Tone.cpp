/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
/**
 * Description: Tone function for Arduino Due and compatible (SAM3X8E)
 * Derived from https://forum.arduino.cc/index.php?topic=136500.msg2903012#msg2903012
 */

#ifdef ARDUINO_ARCH_SAM

#include "../../inc/mvCNCConfig.h"
#include "HAL.h"

static pin_t tone_pin;
volatile static int32_t toggles;

void tone(const pin_t _pin, const unsigned int frequency, const unsigned long duration) {
  tone_pin = _pin;
  toggles = 2 * frequency * duration / 1000;
  HAL_timer_start(MF_TIMER_TONE, 2 * frequency);
}

void noTone(const pin_t _pin) {
  HAL_timer_disable_interrupt(MF_TIMER_TONE);
  extDigitalWrite(_pin, LOW);
}

HAL_TONE_TIMER_ISR() {
  static uint8_t pin_state = 0;
  HAL_timer_isr_prologue(MF_TIMER_TONE);

  if (toggles) {
    toggles--;
    extDigitalWrite(tone_pin, (pin_state ^= 1));
  }
  else noTone(tone_pin);                         // turn off interrupt
}

#endif // ARDUINO_ARCH_SAM
