/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

#include "../../module/endstops.h"

// One ISR for all EXT-Interrupts
void endstop_ISR() { endstops.update(); }

void setup_endstop_interrupts() {
  #define _ATTACH(P) attachInterrupt(P, endstop_ISR, CHANGE)
  TERN_(HAS_X_MAX, _ATTACH(X_MAX_PIN));
  TERN_(HAS_X_MIN, _ATTACH(X_MIN_PIN));
  TERN_(HAS_Y_MAX, _ATTACH(Y_MAX_PIN));
  TERN_(HAS_Y_MIN, _ATTACH(Y_MIN_PIN));
  TERN_(HAS_Z_MAX, _ATTACH(Z_MAX_PIN));
  TERN_(HAS_Z_MIN, _ATTACH(Z_MIN_PIN));
  TERN_(HAS_X2_MAX, _ATTACH(X2_MAX_PIN));
  TERN_(HAS_X2_MIN, _ATTACH(X2_MIN_PIN));
  TERN_(HAS_Y2_MAX, _ATTACH(Y2_MAX_PIN));
  TERN_(HAS_Y2_MIN, _ATTACH(Y2_MIN_PIN));
  TERN_(HAS_Z2_MAX, _ATTACH(Z2_MAX_PIN));
  TERN_(HAS_Z2_MIN, _ATTACH(Z2_MIN_PIN));
  TERN_(HAS_Z3_MAX, _ATTACH(Z3_MAX_PIN));
  TERN_(HAS_Z3_MIN, _ATTACH(Z3_MIN_PIN));
  TERN_(HAS_Z4_MAX, _ATTACH(Z4_MAX_PIN));
  TERN_(HAS_Z4_MIN, _ATTACH(Z4_MIN_PIN));
  TERN_(HAS_Z_MIN_PROBE_PIN, _ATTACH(Z_MIN_PROBE_PIN));
  TERN_(HAS_I_MAX, _ATTACH(I_MAX_PIN));
  TERN_(HAS_I_MIN, _ATTACH(I_MIN_PIN));
  TERN_(HAS_J_MAX, _ATTACH(J_MAX_PIN));
  TERN_(HAS_J_MIN, _ATTACH(J_MIN_PIN));
  TERN_(HAS_K_MAX, _ATTACH(K_MAX_PIN));
  TERN_(HAS_K_MIN, _ATTACH(K_MIN_PIN));
}
