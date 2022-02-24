/**
 * Modern Vintage CNC Firmware
 *
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#include "../../libs/crc16.h"

class PersistentStore {
public:

  // Total available persistent storage space (in bytes)
  static size_t capacity();

  // Prepare to read or write
  static bool access_start();

  // Housecleaning after read or write
  static bool access_finish();

  // Write one or more bytes of data and update the CRC
  // Return 'true' on write error
  static bool write_data(int &pos, const uint8_t *value, size_t size, uint16_t *crc);

  // Read one or more bytes of data and update the CRC
  // Return 'true' on read error
  static bool read_data(int &pos, uint8_t *value, size_t size, uint16_t *crc, const bool writing=true);

  // Write one or more bytes of data
  // Return 'true' on write error
  static bool write_data(const int pos, const uint8_t *value, const size_t size=sizeof(uint8_t)) {
    int data_pos = pos;
    uint16_t crc = 0;
    return write_data(data_pos, value, size, &crc);
  }

  // Write a single byte of data
  // Return 'true' on write error
  static bool write_data(const int pos, const uint8_t value) { return write_data(pos, &value); }

  // Read one or more bytes of data
  // Return 'true' on read error
  static bool read_data(const int pos, uint8_t *value, const size_t size=1) {
    int data_pos = pos;
    uint16_t crc = 0;
    return read_data(data_pos, value, size, &crc);
  }
};

extern PersistentStore persistentStore;
