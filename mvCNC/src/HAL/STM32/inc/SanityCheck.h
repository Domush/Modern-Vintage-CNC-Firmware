/**
 * Modern Vintage CNC Firmware
*/
#pragma once

/**
 * Test STM32-specific configuration values for errors at compile-time.
 */
//#if ENABLED(SPINDLE_LASER_USE_PWM) && !(SPINDLE_LASER_PWM_PIN == 4 || SPINDLE_LASER_PWM_PIN == 6 || SPINDLE_LASER_PWM_PIN == 11)
//  #error "SPINDLE_LASER_PWM_PIN must use SERVO0, SERVO1 or SERVO3 connector"
//#endif


#if ENABLED(SDCARD_EEPROM_EMULATION) && DISABLED(SDSUPPORT)
  #undef SDCARD_EEPROM_EMULATION // Avoid additional error noise
  #if USE_FALLBACK_EEPROM
    #warning "EEPROM type not specified. Fallback is SDCARD_EEPROM_EMULATION."
  #endif
  #error "SDCARD_EEPROM_EMULATION requires SDSUPPORT. Enable SDSUPPORT or choose another EEPROM emulation."
#endif

#if defined(STM32F4xx) && BOTH(JOBCOUNTER, FLASH_EEPROM_EMULATION)
  #warning "FLASH_EEPROM_EMULATION may cause long delays when writing and should not be used while printing."
  #error "Disable JOBCOUNTER or choose another EEPROM emulation."
#endif

#if !defined(STM32F4xx) && ENABLED(FLASH_EEPROM_LEVELING)
  #error "FLASH_EEPROM_LEVELING is currently only supported on STM32F4 hardware."
#endif

#if ENABLED(SERIAL_STATS_MAX_RX_QUEUED)
  #error "SERIAL_STATS_MAX_RX_QUEUED is not supported on STM32."
#elif ENABLED(SERIAL_STATS_DROPPED_RX)
  #error "SERIAL_STATS_DROPPED_RX is not supported on STM32."
#endif

#if ANY(TFT_COLOR_UI, TFT_LVGL_UI, TFT_CLASSIC_UI) && NOT_TARGET(STM32H7xx, STM32F4xx, STM32F1xx)
  #error "TFT_COLOR_UI, TFT_LVGL_UI and TFT_CLASSIC_UI are currently only supported on STM32H7, STM32F4 and STM32F1 hardware."
#endif
