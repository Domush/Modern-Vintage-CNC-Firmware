/**
 * Modern Vintage CNC Firmware
 *
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#ifdef __SAMD51__

#include "../../inc/mvCNCConfig.h"

#if ENABLED(QSPI_EEPROM)

#include "../shared/eeprom_api.h"

#include "QSPIFlash.h"

static bool initialized;

size_t PersistentStore::capacity() { return qspi.size(); }

bool PersistentStore::access_start() {
  if (!initialized) {
    qspi.begin();
    initialized = true;
  }
  return true;
}

bool PersistentStore::access_finish() {
  qspi.flush();
  return true;
}

bool PersistentStore::write_data(int &pos, const uint8_t *value, size_t size, uint16_t *crc) {
  while (size--) {
    const uint8_t v = *value;
    qspi.writeByte(pos, v);
    crc16(crc, &v, 1);
    pos++;
    value++;
  }
  return false;
}

bool PersistentStore::read_data(int &pos, uint8_t *value, size_t size, uint16_t *crc, const bool writing/*=true*/) {
  while (size--) {
    uint8_t c = qspi.readByte(pos);
    if (writing) *value = c;
    crc16(crc, &c, 1);
    pos++;
    value++;
  }
  return false;
}

#endif // QSPI_EEPROM
#endif // __SAMD51__
