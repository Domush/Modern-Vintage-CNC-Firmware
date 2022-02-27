/**
 * Modern Vintage CNC Firmware
*/
#pragma once

/**
 * stepper/trinamic.h
 * Stepper driver indirection for Trinamic
 */

#include <TMCStepper.h>
#if TMCSTEPPER_VERSION < 0x000500
  #error "Update TMCStepper library to 0.5.0 or newer."
#endif

#include "../../inc/mvCNCConfig.h"
#include "../../feature/tmc_util.h"

#define CLASS_TMC2130 TMC2130Stepper
#define CLASS_TMC2160 TMC2160Stepper
#define CLASS_TMC2208 TMC2208Stepper
#define CLASS_TMC2209 TMC2209Stepper
#define CLASS_TMC2660 TMC2660Stepper
#define CLASS_TMC5130 TMC5130Stepper
#define CLASS_TMC5160 TMC5160Stepper

#define TMC_X_LABEL 'X', '0'
#define TMC_Y_LABEL 'Y', '0'
#define TMC_Z_LABEL 'Z', '0'

#define TMC_I_LABEL 'I', '0'
#define TMC_J_LABEL 'J', '0'
#define TMC_K_LABEL 'K', '0'

#define TMC_X2_LABEL 'X', '2'
#define TMC_Y2_LABEL 'Y', '2'
#define TMC_Z2_LABEL 'Z', '2'
#define TMC_Z3_LABEL 'Z', '3'
#define TMC_Z4_LABEL 'Z', '4'

#define __TMC_CLASS(TYPE, L, I, A) TMCmvCNC<CLASS_##TYPE, L, I, A>
#define _TMC_CLASS(TYPE, LandI, A) __TMC_CLASS(TYPE, LandI, A)
#define TMC_CLASS(ST, A) _TMC_CLASS(ST##_DRIVER_TYPE, TMC_##ST##_LABEL, A##_AXIS)

#ifndef CHOPPER_TIMING_X
  #define CHOPPER_TIMING_X CHOPPER_TIMING
#endif
#if HAS_Y_AXIS && !defined(CHOPPER_TIMING_Y)
  #define CHOPPER_TIMING_Y CHOPPER_TIMING
#endif
#if HAS_Z_AXIS && !defined(CHOPPER_TIMING_Z)
  #define CHOPPER_TIMING_Z CHOPPER_TIMING
#endif
#if HAS_I_AXIS && !defined(CHOPPER_TIMING_I)
  #define CHOPPER_TIMING_I CHOPPER_TIMING
#endif
#if HAS_J_AXIS && !defined(CHOPPER_TIMING_J)
  #define CHOPPER_TIMING_J CHOPPER_TIMING
#endif
#if HAS_K_AXIS && !defined(CHOPPER_TIMING_K)
  #define CHOPPER_TIMING_K CHOPPER_TIMING
#endif

#if HAS_TMC220x
  void tmc_serial_begin();
#endif

void restore_trinamic_drivers();
void reset_trinamic_drivers();

#define AXIS_HAS_SQUARE_WAVE(A) (AXIS_IS_TMC(A) && ENABLED(SQUARE_WAVE_STEPPING))

// X Stepper
#if AXIS_IS_TMC(X)
  extern TMC_CLASS(X, X) stepperX;
  static constexpr chopper_timing_t chopper_timing_X = CHOPPER_TIMING_X;
  #if ENABLED(SOFTWARE_DRIVER_ENABLE)
    #define X_ENABLE_INIT() NOOP
    #define X_ENABLE_WRITE(STATE) stepperX.toff((STATE)==X_ENABLE_ON ? chopper_timing_X.toff : 0)
    #define X_ENABLE_READ() stepperX.isEnabled()
  #endif
  #if AXIS_HAS_SQUARE_WAVE(X)
    #define X_STEP_WRITE(STATE) do{ if (STATE) TOGGLE(X_STEP_PIN); }while(0)
  #endif
#endif

// Y Stepper
#if AXIS_IS_TMC(Y)
  extern TMC_CLASS(Y, Y) stepperY;
  static constexpr chopper_timing_t chopper_timing_Y = CHOPPER_TIMING_Y;
  #if ENABLED(SOFTWARE_DRIVER_ENABLE)
    #define Y_ENABLE_INIT() NOOP
    #define Y_ENABLE_WRITE(STATE) stepperY.toff((STATE)==Y_ENABLE_ON ? chopper_timing_Y.toff : 0)
    #define Y_ENABLE_READ() stepperY.isEnabled()
  #endif
  #if AXIS_HAS_SQUARE_WAVE(Y)
    #define Y_STEP_WRITE(STATE) do{ if (STATE) TOGGLE(Y_STEP_PIN); }while(0)
  #endif
#endif

// Z Stepper
#if AXIS_IS_TMC(Z)
  extern TMC_CLASS(Z, Z) stepperZ;
  static constexpr chopper_timing_t chopper_timing_Z = CHOPPER_TIMING_Z;
  #if ENABLED(SOFTWARE_DRIVER_ENABLE)
    #define Z_ENABLE_INIT() NOOP
    #define Z_ENABLE_WRITE(STATE) stepperZ.toff((STATE)==Z_ENABLE_ON ? chopper_timing_Z.toff : 0)
    #define Z_ENABLE_READ() stepperZ.isEnabled()
  #endif
  #if AXIS_HAS_SQUARE_WAVE(Z)
    #define Z_STEP_WRITE(STATE) do{ if (STATE) TOGGLE(Z_STEP_PIN); }while(0)
  #endif
#endif

// X2 Stepper
#if HAS_X2_ENABLE && AXIS_IS_TMC(X2)
  extern TMC_CLASS(X2, X) stepperX2;
  #ifndef CHOPPER_TIMING_X2
    #define CHOPPER_TIMING_X2 CHOPPER_TIMING_X
  #endif
  static constexpr chopper_timing_t chopper_timing_X2 = CHOPPER_TIMING_X2;
  #if ENABLED(SOFTWARE_DRIVER_ENABLE)
    #define X2_ENABLE_INIT() NOOP
    #define X2_ENABLE_WRITE(STATE) stepperX2.toff((STATE)==X_ENABLE_ON ? chopper_timing_X2.toff : 0)
    #define X2_ENABLE_READ() stepperX2.isEnabled()
  #endif
  #if AXIS_HAS_SQUARE_WAVE(X2)
    #define X2_STEP_WRITE(STATE) do{ if (STATE) TOGGLE(X2_STEP_PIN); }while(0)
  #endif
#endif

// Y2 Stepper
#if HAS_Y2_ENABLE && AXIS_IS_TMC(Y2)
  extern TMC_CLASS(Y2, Y) stepperY2;
  #ifndef CHOPPER_TIMING_Y2
    #define CHOPPER_TIMING_Y2 CHOPPER_TIMING_Y
  #endif
  static constexpr chopper_timing_t chopper_timing_Y2 = CHOPPER_TIMING_Y2;
  #if ENABLED(SOFTWARE_DRIVER_ENABLE)
    #define Y2_ENABLE_INIT() NOOP
    #define Y2_ENABLE_WRITE(STATE) stepperY2.toff((STATE)==Y_ENABLE_ON ? chopper_timing_Y2.toff : 0)
    #define Y2_ENABLE_READ() stepperY2.isEnabled()
  #endif
  #if AXIS_HAS_SQUARE_WAVE(Y2)
    #define Y2_STEP_WRITE(STATE) do{ if (STATE) TOGGLE(Y2_STEP_PIN); }while(0)
  #endif
#endif

// Z2 Stepper
#if HAS_Z2_ENABLE && AXIS_IS_TMC(Z2)
  extern TMC_CLASS(Z2, Z) stepperZ2;
  #ifndef CHOPPER_TIMING_Z2
    #define CHOPPER_TIMING_Z2 CHOPPER_TIMING_Z
  #endif
  static constexpr chopper_timing_t chopper_timing_Z2 = CHOPPER_TIMING_Z2;
  #if ENABLED(SOFTWARE_DRIVER_ENABLE)
    #define Z2_ENABLE_INIT() NOOP
    #define Z2_ENABLE_WRITE(STATE) stepperZ2.toff((STATE)==Z_ENABLE_ON ? chopper_timing_Z2.toff : 0)
    #define Z2_ENABLE_READ() stepperZ2.isEnabled()
  #endif
  #if AXIS_HAS_SQUARE_WAVE(Z2)
    #define Z2_STEP_WRITE(STATE) do{ if (STATE) TOGGLE(Z2_STEP_PIN); }while(0)
  #endif
#endif

// Z3 Stepper
#if HAS_Z3_ENABLE && AXIS_IS_TMC(Z3)
  extern TMC_CLASS(Z3, Z) stepperZ3;
  #ifndef CHOPPER_TIMING_Z3
    #define CHOPPER_TIMING_Z3 CHOPPER_TIMING_Z
  #endif
  static constexpr chopper_timing_t chopper_timing_Z3 = CHOPPER_TIMING_Z3;
  #if ENABLED(SOFTWARE_DRIVER_ENABLE)
    #define Z3_ENABLE_INIT() NOOP
    #define Z3_ENABLE_WRITE(STATE) stepperZ3.toff((STATE)==Z_ENABLE_ON ? chopper_timing_Z3.toff : 0)
    #define Z3_ENABLE_READ() stepperZ3.isEnabled()
  #endif
  #if AXIS_HAS_SQUARE_WAVE(Z3)
    #define Z3_STEP_WRITE(STATE) do{ if (STATE) TOGGLE(Z3_STEP_PIN); }while(0)
  #endif
#endif

// Z4 Stepper
#if HAS_Z4_ENABLE && AXIS_IS_TMC(Z4)
  extern TMC_CLASS(Z4, Z) stepperZ4;
  #ifndef CHOPPER_TIMING_Z4
    #define CHOPPER_TIMING_Z4 CHOPPER_TIMING_Z
  #endif
  static constexpr chopper_timing_t chopper_timing_Z4 = CHOPPER_TIMING_Z4;
  #if ENABLED(SOFTWARE_DRIVER_ENABLE)
    #define Z4_ENABLE_INIT() NOOP
    #define Z4_ENABLE_WRITE(STATE) stepperZ4.toff((STATE)==Z_ENABLE_ON ? chopper_timing_Z4.toff : 0)
    #define Z4_ENABLE_READ() stepperZ4.isEnabled()
  #endif
  #if AXIS_HAS_SQUARE_WAVE(Z4)
    #define Z4_STEP_WRITE(STATE) do{ if (STATE) TOGGLE(Z4_STEP_PIN); }while(0)
  #endif
#endif

// I Stepper
#if AXIS_IS_TMC(I)
  extern TMC_CLASS(I, I) stepperI;
  static constexpr chopper_timing_t chopper_timing_I = CHOPPER_TIMING_I;
  #if ENABLED(SOFTWARE_DRIVER_ENABLE)
    #define I_ENABLE_INIT() NOOP
    #define I_ENABLE_WRITE(STATE) stepperI.toff((STATE)==I_ENABLE_ON ? chopper_timing.toff : 0)
    #define I_ENABLE_READ() stepperI.isEnabled()
  #endif
  #if AXIS_HAS_SQUARE_WAVE(I)
    #define I_STEP_WRITE(STATE) do{ if (STATE) TOGGLE(I_STEP_PIN); }while(0)
  #endif
#endif

// J Stepper
#if AXIS_IS_TMC(J)
  extern TMC_CLASS(J, J) stepperJ;
  static constexpr chopper_timing_t chopper_timing_J = CHOPPER_TIMING_J;
  #if ENABLED(SOFTWARE_DRIVER_ENABLE)
    #define J_ENABLE_INIT() NOOP
    #define J_ENABLE_WRITE(STATE) stepperJ.toff((STATE)==J_ENABLE_ON ? chopper_timing.toff : 0)
    #define J_ENABLE_READ() stepperJ.isEnabled()
  #endif
  #if AXIS_HAS_SQUARE_WAVE(J)
    #define J_STEP_WRITE(STATE) do{ if (STATE) TOGGLE(J_STEP_PIN); }while(0)
  #endif
#endif

// K Stepper
#if AXIS_IS_TMC(K)
  extern TMC_CLASS(K, K) stepperK;
  static constexpr chopper_timing_t chopper_timing_K = CHOPPER_TIMING_K;
  #if ENABLED(SOFTWARE_DRIVER_ENABLE)
    #define K_ENABLE_INIT() NOOP
    #define K_ENABLE_WRITE(STATE) stepperK.toff((STATE)==K_ENABLE_ON ? chopper_timing.toff : 0)
    #define K_ENABLE_READ() stepperK.isEnabled()
  #endif
  #if AXIS_HAS_SQUARE_WAVE(K)
    #define K_STEP_WRITE(STATE) do{ if (STATE) TOGGLE(K_STEP_PIN); }while(0)
  #endif
#endif
