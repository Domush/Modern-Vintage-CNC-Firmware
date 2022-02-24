/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#include "../../../inc/mvCNCConfig.h"

#if ENABLED(HAS_MCP3426_ADC)

#include "../../gcode.h"

#include "../../../feature/adc/adc_mcp3426.h"

/**
 * M3426: Read 16 bit (signed) value from I2C MCP3426 ADC device
 *
 *  M3426 C<byte-1 value in base 10> channel 1 or 2
 *  M3426 G<byte-1 value in base 10> gain 1, 2, 4 or 8
 *  M3426 I<byte-2 value in base 10> 0 or 1, invert reply
 */
void GcodeSuite::M3426() {
  uint8_t channel = parser.byteval('C', 1),       // Select the channel 1 or 2
             gain = parser.byteval('G', 1);
  const bool inverted = parser.byteval('I') == 1;

  if (channel <= 2 && (gain == 1 || gain == 2 || gain == 4 || gain == 8)) {
    int16_t result = mcp3426.ReadValue(channel, gain);

    if (mcp3426.Error == false) {
      if (inverted) {
        // Should we invert the reading (32767 - ADC value) ?
        // Caters to end devices that expect values to increase when in reality they decrease.
        // e.g., A pressure sensor in a vacuum when the end device expects a positive pressure.
        result = INT16_MAX - result;
      }
      //SERIAL_ECHOPGM(STR_OK);
      SERIAL_ECHOLNPGM("V:", result, " C:", channel, " G:", gain, " I:", inverted ? 1 : 0);
    }
    else
      SERIAL_ERROR_MSG("MCP342X i2c error");
  }
  else
    SERIAL_ERROR_MSG("MCP342X Bad request");
}

#endif // HAS_MCP3426_ADC
