/**
 * Modern Vintage CNC Firmware
*/
#pragma once

#if USE_FALLBACK_EEPROM
  #define FLASH_EEPROM_EMULATION
#elif EITHER(I2C_EEPROM, SPI_EEPROM)
  #define USE_SHARED_EEPROM 1
#endif

// LPC1768 boards seem to lose steps when saving to EEPROM during a CNC job (issue #20785)
// TODO: Which other boards are incompatible?
#if defined(MCU_LPC1768) && JOBCOUNTER_SAVE_INTERVAL > 0
  #define JOBCOUNTER_SYNC 1
#endif
