/**
 * Modern Vintage CNC Firmware
 *
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
/**
 * Test SAMD51 specific configuration values for errors at compile-time.
 */

#if ENABLED(FLASH_EEPROM_EMULATION)
  #warning "Did you activate the SmartEEPROM? See https://github.com/GMagician/SAMD51-SmartEEprom-Manager/releases"
#endif

#if defined(ADAFRUIT_GRAND_CENTRAL_M4) && SD_CONNECTION_IS(CUSTOM_CABLE)
  #error "No custom SD drive cable defined for this board."
#endif

#if (defined(TEMP_0_SCK_PIN) && defined(TEMP_0_MISO_PIN) && (TEMP_0_SCK_PIN == SCK1 || TEMP_0_MISO_PIN == MISO1)) || \
    (defined(TEMP_1_SCK_PIN) && defined(TEMP_1_MISO_PIN) && (TEMP_1_SCK_PIN == SCK1 || TEMP_1_MISO_PIN == MISO1))
  #error "OnBoard SPI BUS can't be shared with other devices."
#endif

#if SERVO_TC == MF_TIMER_RTC
  #error "Servos can't use RTC timer"
#endif

#if ENABLED(EMERGENCY_PARSER)
  #error "EMERGENCY_PARSER is not yet implemented for SAMD51. Disable EMERGENCY_PARSER to continue."
#endif

#if ENABLED(SDIO_SUPPORT)
  #error "SDIO_SUPPORT is not supported on SAMD51."
#endif

#if ENABLED(FAST_PWM_FAN) || SPINDLE_LASER_FREQUENCY
  #error "Features requiring Hardware PWM (FAST_PWM_FAN, SPINDLE_LASER_FREQUENCY) are not yet supported on SAMD51."
#endif

#if ENABLED(POSTMORTEM_DEBUGGING)
  #error "POSTMORTEM_DEBUGGING is not yet supported on AGCM4."
#endif