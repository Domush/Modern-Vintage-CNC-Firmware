/**
 * Modern Vintage CNC Firmware
*/
#pragma once

/**
 * Test Arduino Due specific configuration values for errors at compile-time.
 */

/**
 * HARDWARE VS. SOFTWARE SPI COMPATIBILITY
 *
 * DUE selects hardware vs. software SPI depending on whether one of the hardware-controllable SDSS pins is in use.
 *
 * The hardware SPI controller doesn't allow software SPIs to control any shared pins.
 *
 * When DUE software SPI is used then Trinamic drivers must use the TMC softSPI.
 *
 * When DUE hardware SPI is used then a Trinamic driver can use either its hardware SPI or, if there are no shared
 * pins, its software SPI.
 *
 * Usually the hardware SPI pins are only available to the LCD. This makes the DUE hard SPI used at the same time
 * as the TMC2130 soft SPI the most common setup.
 */
#define _IS_HW_SPI(P) (defined(TMC_SW_##P) && (TMC_SW_##P == SD_MOSI_PIN || TMC_SW_##P == SD_MISO_PIN || TMC_SW_##P == SD_SCK_PIN))

#if ENABLED(SDSUPPORT) && HAS_DRIVER(TMC2130)
  #if ENABLED(TMC_USE_SW_SPI)
    #if DISABLED(DUE_SOFTWARE_SPI) && (_IS_HW_SPI(MOSI) || _IS_HW_SPI(MISO) || _IS_HW_SPI(SCK))
      #error "DUE hardware SPI is required but is incompatible with TMC2130 software SPI. Either disable TMC_USE_SW_SPI or use separate pins for the two SPIs."
    #endif
  #elif ENABLED(DUE_SOFTWARE_SPI)
    #error "DUE software SPI is required but is incompatible with TMC2130 hardware SPI. Enable TMC_USE_SW_SPI to fix."
  #endif
#endif

#if ENABLED(FAST_PWM_FAN) || SPINDLE_LASER_FREQUENCY
  #error "Features requiring Hardware PWM (FAST_PWM_FAN, SPINDLE_LASER_FREQUENCY) are not yet supported on DUE."
#endif

#if HAS_TMC_SW_SERIAL
  #error "TMC220x Software Serial is not supported on the DUE platform."
#endif
