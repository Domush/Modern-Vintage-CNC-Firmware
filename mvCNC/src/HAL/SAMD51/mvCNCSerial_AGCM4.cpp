/**
 * Modern Vintage CNC Firmware
 *
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#ifdef ADAFRUIT_GRAND_CENTRAL_M4

/**
 * Framework doesn't define some serials to save sercom resources
 * hence if these are used I need to define them
 */

#include "../../inc/mvCNCConfig.h"

#if USING_HW_SERIAL1
  UartT Serial2(false, &sercom4, PIN_SERIAL2_RX, PIN_SERIAL2_TX, PAD_SERIAL2_RX, PAD_SERIAL2_TX);
  void SERCOM4_0_Handler() { Serial2.IrqHandler(); }
  void SERCOM4_1_Handler() { Serial2.IrqHandler(); }
  void SERCOM4_2_Handler() { Serial2.IrqHandler(); }
  void SERCOM4_3_Handler() { Serial2.IrqHandler(); }
#endif

#if USING_HW_SERIAL2
  UartT Serial3(false, &sercom1, PIN_SERIAL3_RX, PIN_SERIAL3_TX, PAD_SERIAL3_RX, PAD_SERIAL3_TX);
  void SERCOM1_0_Handler() { Serial3.IrqHandler(); }
  void SERCOM1_1_Handler() { Serial3.IrqHandler(); }
  void SERCOM1_2_Handler() { Serial3.IrqHandler(); }
  void SERCOM1_3_Handler() { Serial3.IrqHandler(); }
#endif

#if USING_HW_SERIAL3
  UartT Serial4(false, &sercom5, PIN_SERIAL4_RX, PIN_SERIAL4_TX, PAD_SERIAL4_RX, PAD_SERIAL4_TX);
  void SERCOM5_0_Handler() { Serial4.IrqHandler(); }
  void SERCOM5_1_Handler() { Serial4.IrqHandler(); }
  void SERCOM5_2_Handler() { Serial4.IrqHandler(); }
  void SERCOM5_3_Handler() { Serial4.IrqHandler(); }
#endif

#endif // ADAFRUIT_GRAND_CENTRAL_M4
