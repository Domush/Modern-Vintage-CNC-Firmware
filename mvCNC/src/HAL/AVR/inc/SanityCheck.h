/**
 * Modern Vintage CNC Firmware
*/
#pragma once

/**
 * Test AVR-specific configuration values for errors at compile-time.
 */

/**
 * Checks for FAST PWM
 */
#if ALL(FAST_PWM_FAN, USE_OCR2A_AS_TOP, HAS_TCCR2)
  #error "USE_OCR2A_AS_TOP does not apply to devices with a single output TIMER2."
#endif

/**
 * Checks for SOFT PWM
 */
#if HAS_FAN0 && FAN_PIN == 9 && DISABLED(FAN_SOFT_PWM) && ENABLED(SPEAKER)
  #error "FAN_PIN 9 Hardware PWM uses Timer 2 which conflicts with Arduino AVR Tone Timer (for SPEAKER)."
  #error "Disable SPEAKER or enable FAN_SOFT_PWM."
#endif

/**
 * Sanity checks for Spindle / Laser PWM
 */
#if ENABLED(SPINDLE_LASER_USE_PWM)
  #include "../ServoTimers.h"   // Needed to check timer availability (_useTimer3)
  #if SPINDLE_LASER_PWM_PIN == 4 || WITHIN(SPINDLE_LASER_PWM_PIN, 11, 13)
    #error "Counter/Timer for SPINDLE_LASER_PWM_PIN is used by a system interrupt."
  #elif NUM_SERVOS > 0 && defined(_useTimer3) && (WITHIN(SPINDLE_LASER_PWM_PIN, 2, 3) || SPINDLE_LASER_PWM_PIN == 5)
    #error "Counter/Timer for SPINDLE_LASER_PWM_PIN is used by the servo system."
  #endif
#elif SPINDLE_LASER_FREQUENCY
  #error "SPINDLE_LASER_FREQUENCY requires SPINDLE_LASER_USE_PWM."
#endif

/**
 * The Trinamic library includes SoftwareSerial.h, leading to a compile error.
 */
#if BOTH(HAS_TRINAMIC_CONFIG, ENDSTOP_INTERRUPTS_FEATURE)
  #error "TMCStepper includes SoftwareSerial.h which is incompatible with ENDSTOP_INTERRUPTS_FEATURE. Disable ENDSTOP_INTERRUPTS_FEATURE to continue."
#endif

#if BOTH(HAS_TMC_SW_SERIAL, MONITOR_DRIVER_STATUS)
  #error "MONITOR_DRIVER_STATUS causes performance issues when used with SoftwareSerial-connected drivers. Disable MONITOR_DRIVER_STATUS or use hardware serial to continue."
#endif

/**
 * Postmortem debugging
 */
#if ENABLED(POSTMORTEM_DEBUGGING)
  #error "POSTMORTEM_DEBUGGING is not supported on AVR boards."
#endif
