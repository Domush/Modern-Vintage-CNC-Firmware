/**
 * Modern Vintage CNC Firmware
 *
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
/**
 * persistent_store_flash.cpp
 * HAL for stm32duino and compatible (STM32F1)
 * Implementation of EEPROM settings in SDCard
 */

#ifdef __STM32F1__

#include "../../inc/mvCNCConfig.h"

#if ENABLED(FLASH_EEPROM_EMULATION)

#include "../shared/eeprom_api.h"

#include <flash_stm32.h>
#include <EEPROM.h>

// Store settings in the last two pages
    #ifndef MVCNC_EEPROM_SIZE
      #define MVCNC_EEPROM_SIZE ((EEPROM_PAGE_SIZE)*2)
    #endif
size_t PersistentStore::capacity() {
  return MVCNC_EEPROM_SIZE;
}

static uint8_t ram_eeprom[MVCNC_EEPROM_SIZE] __attribute__((aligned(4))) = {0};
static bool eeprom_dirty = false;

bool PersistentStore::access_start() {
  const uint32_t *source = reinterpret_cast<const uint32_t*>(EEPROM_PAGE0_BASE);
  uint32_t *destination = reinterpret_cast<uint32_t*>(ram_eeprom);

  static_assert(0 == (MVCNC_EEPROM_SIZE) % 4,
                "MVCNC_EEPROM_SIZE is corrupted. (Must be a multiple of 4.)");  // Ensure copying as uint32_t is safe
  constexpr size_t eeprom_size_u32 = (MVCNC_EEPROM_SIZE) / 4;

  for (size_t i = 0; i < eeprom_size_u32; ++i, ++destination, ++source)
    *destination = *source;

  eeprom_dirty = false;
  return true;
}

bool PersistentStore::access_finish() {

  if (eeprom_dirty) {
    FLASH_Status status;

    // Instead of erasing all (both) pages, maybe in the loop we check what page we are in, and if the
    // data has changed in that page. We then erase the first time we "detect" a change. In theory, if
    // nothing changed in a page, we wouldn't need to erase/write it.
    // Or, instead of checking at this point, turn eeprom_dirty into an array of bool the size of number
    // of pages. Inside write_data, we set the flag to true at that time if something in that
    // page changes...either way, something to look at later.
    FLASH_Unlock();

    #define ACCESS_FINISHED(TF) { FLASH_Lock(); eeprom_dirty = false; return TF; }

    status = FLASH_ErasePage(EEPROM_PAGE0_BASE);
    if (status != FLASH_COMPLETE) ACCESS_FINISHED(true);
    status = FLASH_ErasePage(EEPROM_PAGE1_BASE);
    if (status != FLASH_COMPLETE) ACCESS_FINISHED(true);

    const uint16_t *source = reinterpret_cast<const uint16_t*>(ram_eeprom);
    for (size_t i = 0; i < MVCNC_EEPROM_SIZE; i += 2, ++source) {
      if (FLASH_ProgramHalfWord(EEPROM_PAGE0_BASE + i, *source) != FLASH_COMPLETE)
        ACCESS_FINISHED(false);
    }

    ACCESS_FINISHED(true);
  }

  return true;
}

bool PersistentStore::write_data(int &pos, const uint8_t *value, size_t size, uint16_t *crc) {
  for (size_t i = 0; i < size; ++i) ram_eeprom[pos + i] = value[i];
  eeprom_dirty = true;
  crc16(crc, value, size);
  pos += size;
  return false;  // return true for any error
}

bool PersistentStore::read_data(int &pos, uint8_t *value, const size_t size, uint16_t *crc, const bool writing/*=true*/) {
  const uint8_t * const buff = writing ? &value[0] : &ram_eeprom[pos];
  if (writing) for (size_t i = 0; i < size; i++) value[i] = ram_eeprom[pos + i];
  crc16(crc, buff, size);
  pos += size;
  return false;  // return true for any error
}

#endif // FLASH_EEPROM_EMULATION
#endif // __STM32F1__