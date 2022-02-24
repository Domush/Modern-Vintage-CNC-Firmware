/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

/**
 * Arduino library for MicroChip MCP3426 I2C A/D converter.
 * https://www.microchip.com/en-us/product/MCP3426
 */

#include <stdint.h>
#include <Wire.h>

// Address of MCP342X chip
#define MCP342X_ADC_I2C_ADDRESS 104

class MCP3426 {
  public:
    int16_t ReadValue(uint8_t channel, uint8_t gain);
    bool Error;
};

extern MCP3426 mcp3426;
