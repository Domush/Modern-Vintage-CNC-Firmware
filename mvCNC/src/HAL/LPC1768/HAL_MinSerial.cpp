/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#ifdef TARGET_LPC1768

#include "../../inc/mvCNCConfig.h"
#include "HAL.h"

#if ENABLED(POSTMORTEM_DEBUGGING)

#include "../shared/HAL_MinSerial.h"
#include <debug_frmwrk.h>

static void TX(char c) { _DBC(c); }
void install_min_serial() { HAL_min_serial_out = &TX; }

#if DISABLED(DYNAMIC_VECTORTABLE)
extern "C" {
  __attribute__((naked)) void JumpHandler_ASM() {
    __asm__ __volatile__ (
      "b CommonHandler_ASM\n"
    );
  }
  void __attribute__((naked, alias("JumpHandler_ASM"))) HardFault_Handler();
  void __attribute__((naked, alias("JumpHandler_ASM"))) BusFault_Handler();
  void __attribute__((naked, alias("JumpHandler_ASM"))) UsageFault_Handler();
  void __attribute__((naked, alias("JumpHandler_ASM"))) MemManage_Handler();
  void __attribute__((naked, alias("JumpHandler_ASM"))) NMI_Handler();
}
#endif

#endif // POSTMORTEM_DEBUGGING
#endif // TARGET_LPC1768
