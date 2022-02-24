/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

/**
 * Test X86_64-specific configuration values for errors at compile-time.
 */

// Emulating RAMPS
#if ENABLED(SPINDLE_LASER_USE_PWM) && !(SPINDLE_LASER_PWM_PIN == 4 || SPINDLE_LASER_PWM_PIN == 6 || SPINDLE_LASER_PWM_PIN == 11)
  #error "SPINDLE_LASER_PWM_PIN must use SERVO0, SERVO1 or SERVO3 connector"
#endif

#if ENABLED(FAST_PWM_FAN) || SPINDLE_LASER_FREQUENCY
  #error "Features requiring Hardware PWM (FAST_PWM_FAN, SPINDLE_LASER_FREQUENCY) are not yet supported on LINUX."
#endif

#if HAS_TMC_SW_SERIAL
  #error "TMC220x Software Serial is not supported on LINUX."
#endif

#if ENABLED(POSTMORTEM_DEBUGGING)
  #error "POSTMORTEM_DEBUGGING is not yet supported on LINUX."
#endif
