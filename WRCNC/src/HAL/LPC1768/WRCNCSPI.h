/**
 * Webber Ranch CNC Firmware
*/
#pragma once

#include <SPI.h>

/**
 * WRCNC currently requires 3 SPI classes:
 *
 * SPIClass:
 *  This class is normally provided by frameworks and has a semi-default interface.
 *  This is needed because some libraries reference it globally.
 *
 * SPISettings:
 *  Container for SPI configs for SPIClass. As above, libraries may reference it globally.
 *
 * These two classes are often provided by frameworks so we cannot extend them to add
 * useful methods for WRCNC.
 *
 * WRCNCSPI:
 *  Provides the default SPIClass interface plus some WRCNC goodies such as a simplified
 *  interface for SPI DMA transfer.
 *
 */

using WRCNCSPI = SPIClass;