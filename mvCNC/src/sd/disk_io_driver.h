/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

#include <stdint.h>

/**
 * DiskIO Interface
 *
 * Interface for low level disk io
 */
class DiskIODriver {
public:
  /**
   * Initialize an SD flash memory card with default clock rate and chip
   * select pin.  See sd2Card::init(uint8_t sckRateID, uint8_t chipSelectPin).
   *
   * \return true for success or false for failure.
   */
  virtual bool init(const uint8_t sckRateID, const pin_t chipSelectPin) = 0; //TODO: only for SPI

  /**
   * Read a card's CSD register. The CSD contains Card-Specific Data that
   * provides information regarding access to the card's contents.
   *
   * \param[out] csd pointer to area for returned data.
   *
   * \return true for success or false for failure.
   */
  virtual bool readCSD(csd_t* csd) = 0;

  virtual bool readStart(const uint32_t block) = 0;
  virtual bool readData(uint8_t* dst) = 0;
  virtual bool readStop() = 0;

  virtual bool writeStart(const uint32_t block, const uint32_t) = 0;
  virtual bool writeData(const uint8_t* src) = 0;
  virtual bool writeStop() = 0;

  virtual bool readBlock(uint32_t block, uint8_t* dst) = 0;
  virtual bool writeBlock(uint32_t blockNumber, const uint8_t* src) = 0;

  virtual uint32_t cardSize() = 0;

  virtual bool isReady() = 0;

  virtual void idle() = 0;
};
