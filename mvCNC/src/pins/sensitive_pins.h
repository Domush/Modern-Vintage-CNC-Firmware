/**
 * Modern Vintage CNC Firmware
*/
#pragma once
//
// Prepare a list of protected pins for M42/M43
//

#if PIN_EXISTS(X_MIN)
  #define _X_MIN X_MIN_PIN,
#else
  #define _X_MIN
#endif
#if PIN_EXISTS(X_MAX)
  #define _X_MAX X_MAX_PIN,
#else
  #define _X_MAX
#endif
#if PIN_EXISTS(X_CS) && AXIS_HAS_SPI(X)
  #define _X_CS X_CS_PIN,
#else
  #define _X_CS
#endif
#if PIN_EXISTS(X_MS1)
  #define _X_MS1 X_MS1_PIN,
#else
  #define _X_MS1
#endif
#if PIN_EXISTS(X_MS2)
  #define _X_MS2 X_MS2_PIN,
#else
  #define _X_MS2
#endif
#if PIN_EXISTS(X_MS3)
  #define _X_MS3 X_MS3_PIN,
#else
  #define _X_MS3
#endif
#if PIN_EXISTS(X_ENABLE)
  #define _X_ENABLE_PIN X_ENABLE_PIN,
#else
  #define _X_ENABLE_PIN
#endif

#define _X_PINS X_STEP_PIN, X_DIR_PIN, _X_ENABLE_PIN _X_MIN _X_MAX _X_MS1 _X_MS2 _X_MS3 _X_CS

#if HAS_Y_AXIS

  #if PIN_EXISTS(Y_MIN)
    #define _Y_MIN Y_MIN_PIN,
  #else
    #define _Y_MIN
  #endif
  #if PIN_EXISTS(Y_MAX)
    #define _Y_MAX Y_MAX_PIN,
  #else
    #define _Y_MAX
  #endif
  #if PIN_EXISTS(Y_CS) && AXIS_HAS_SPI(Y)
    #define _Y_CS Y_CS_PIN,
  #else
    #define _Y_CS
  #endif
  #if PIN_EXISTS(Y_MS1)
    #define _Y_MS1 Y_MS1_PIN,
  #else
    #define _Y_MS1
  #endif
  #if PIN_EXISTS(Y_MS2)
    #define _Y_MS2 Y_MS2_PIN,
  #else
    #define _Y_MS2
  #endif
  #if PIN_EXISTS(Y_MS3)
    #define _Y_MS3 Y_MS3_PIN,
  #else
    #define _Y_MS3
  #endif
  #if PIN_EXISTS(Y_ENABLE)
    #define _Y_ENABLE_PIN Y_ENABLE_PIN,
  #else
    #define _Y_ENABLE_PIN
  #endif

  #define _Y_PINS Y_STEP_PIN, Y_DIR_PIN, _Y_ENABLE_PIN _Y_MIN _Y_MAX _Y_MS1 _Y_MS2 _Y_MS3 _Y_CS

#else

  #define _Y_PINS

#endif

#if HAS_Z_AXIS

  #if PIN_EXISTS(Z_MIN)
    #define _Z_MIN Z_MIN_PIN,
  #else
    #define _Z_MIN
  #endif
  #if PIN_EXISTS(Z_MAX)
    #define _Z_MAX Z_MAX_PIN,
  #else
    #define _Z_MAX
  #endif
  #if PIN_EXISTS(Z_CS) && AXIS_HAS_SPI(Z)
    #define _Z_CS Z_CS_PIN,
  #else
    #define _Z_CS
  #endif
  #if PIN_EXISTS(Z_MS1)
    #define _Z_MS1 Z_MS1_PIN,
  #else
    #define _Z_MS1
  #endif
  #if PIN_EXISTS(Z_MS2)
    #define _Z_MS2 Z_MS2_PIN,
  #else
    #define _Z_MS2
  #endif
  #if PIN_EXISTS(Z_MS3)
    #define _Z_MS3 Z_MS3_PIN,
  #else
    #define _Z_MS3
  #endif
  #if PIN_EXISTS(Z_ENABLE)
    #define _Z_ENABLE_PIN Z_ENABLE_PIN,
  #else
    #define _Z_ENABLE_PIN
  #endif

  #define _Z_PINS Z_STEP_PIN, Z_DIR_PIN, _Z_ENABLE_PIN _Z_MIN _Z_MAX _Z_MS1 _Z_MS2 _Z_MS3 _Z_CS

#else

  #define _Z_PINS

#endif

#if HAS_I_AXIS

  #if PIN_EXISTS(I_MIN)
    #define _I_MIN I_MIN_PIN,
  #else
    #define _I_MIN
  #endif
  #if PIN_EXISTS(I_MAX)
    #define _I_MAX I_MAX_PIN,
  #else
    #define _I_MAX
  #endif
  #if PIN_EXISTS(I_CS) && AXIS_HAS_SPI(I)
    #define _I_CS I_CS_PIN,
  #else
    #define _I_CS
  #endif
  #if PIN_EXISTS(I_MS1)
    #define _I_MS1 I_MS1_PIN,
  #else
    #define _I_MS1
  #endif
  #if PIN_EXISTS(I_MS2)
    #define _I_MS2 I_MS2_PIN,
  #else
    #define _I_MS2
  #endif
  #if PIN_EXISTS(I_MS3)
    #define _I_MS3 I_MS3_PIN,
  #else
    #define _I_MS3
  #endif
  #if PIN_EXISTS(I_ENABLE)
    #define _I_ENABLE_PIN I_ENABLE_PIN,
  #else
    #define _I_ENABLE_PIN
  #endif

  #define _I_PINS I_STEP_PIN, I_DIR_PIN, _I_ENABLE_PIN _I_MIN _I_MAX _I_MS1 _I_MS2 _I_MS3 _I_CS

#else

  #define _I_PINS

#endif

#if HAS_J_AXIS

  #if PIN_EXISTS(J_MIN)
    #define _J_MIN J_MIN_PIN,
  #else
    #define _J_MIN
  #endif
  #if PIN_EXISTS(J_MAX)
    #define _J_MAX J_MAX_PIN,
  #else
    #define _J_MAX
  #endif
  #if PIN_EXISTS(J_CS) && AXIS_HAS_SPI(J)
    #define _J_CS J_CS_PIN,
  #else
    #define _J_CS
  #endif
  #if PIN_EXISTS(J_MS1)
    #define _J_MS1 J_MS1_PIN,
  #else
    #define _J_MS1
  #endif
  #if PIN_EXISTS(J_MS2)
    #define _J_MS2 J_MS2_PIN,
  #else
    #define _J_MS2
  #endif
  #if PIN_EXISTS(J_MS3)
    #define _J_MS3 J_MS3_PIN,
  #else
    #define _J_MS3
  #endif
  #if PIN_EXISTS(J_ENABLE)
    #define _J_ENABLE_PIN J_ENABLE_PIN,
  #else
    #define _J_ENABLE_PIN
  #endif

  #define _J_PINS J_STEP_PIN, J_DIR_PIN, _J_ENABLE_PIN _J_MIN _J_MAX _J_MS1 _J_MS2 _J_MS3 _J_CS

#else

  #define _J_PINS

#endif

#if HAS_K_AXIS

  #if PIN_EXISTS(K_MIN)
    #define _K_MIN K_MIN_PIN,
  #else
    #define _K_MIN
  #endif
  #if PIN_EXISTS(K_MAX)
    #define _K_MAX K_MAX_PIN,
  #else
    #define _K_MAX
  #endif
  #if PIN_EXISTS(K_CS) && AXIS_HAS_SPI(K)
    #define _K_CS K_CS_PIN,
  #else
    #define _K_CS
  #endif
  #if PIN_EXISTS(K_MS1)
    #define _K_MS1 K_MS1_PIN,
  #else
    #define _K_MS1
  #endif
  #if PIN_EXISTS(K_MS2)
    #define _K_MS2 K_MS2_PIN,
  #else
    #define _K_MS2
  #endif
  #if PIN_EXISTS(K_MS3)
    #define _K_MS3 K_MS3_PIN,
  #else
    #define _K_MS3
  #endif
  #if PIN_EXISTS(K_ENABLE)
    #define _K_ENABLE_PIN K_ENABLE_PIN,
  #else
    #define _K_ENABLE_PIN
  #endif

  #define _K_PINS K_STEP_PIN, K_DIR_PIN, _K_ENABLE_PIN _K_MIN _K_MAX _K_MS1 _K_MS2 _K_MS3 _K_CS

#else

  #define _K_PINS

#endif

//
// For boards with Extruder driver slots, this will make those pins available to other axes
//
#define _E0_CS
#define _E0_MS1
#define _E0_MS2
#define _E0_MS3
#define _E1_CS
#define _E1_MS1
#define _E1_MS2
#define _E1_MS3
#define _E2_CS
#define _E2_MS1
#define _E2_MS2
#define _E2_MS3
#define _E3_CS
#define _E3_MS1
#define _E3_MS2
#define _E3_MS3
#define _E4_CS
#define _E4_MS1
#define _E4_MS2
#define _E4_MS3
#define _E5_CS
#define _E5_MS1
#define _E5_MS2
#define _E5_MS3
#define _E6_CS
#define _E6_MS1
#define _E6_MS2
#define _E6_MS3
#define _E7_CS
#define _E7_MS1
#define _E7_MS2
#define _E7_MS3
#define _E0_PINS
#define _E1_PINS
#define _E2_PINS
#define _E3_PINS
#define _E4_PINS
#define _E5_PINS
#define _E6_PINS
#define _E7_PINS

//
// Make temp sensor and fan pins available
//

#define E0_AUTO_FAN_PIN -1
#define E1_AUTO_FAN_PIN -1
#define E2_AUTO_FAN_PIN -1
#define E3_AUTO_FAN_PIN -1
#define E4_AUTO_FAN_PIN -1
#define E5_AUTO_FAN_PIN -1
#define E6_AUTO_FAN_PIN -1
#define E7_AUTO_FAN_PIN -1


//
// Make heaters PWM pins available
//

#define _H0_PINS
#define _H1_PINS
#define _H2_PINS
#define _H3_PINS
#define _H4_PINS
#define _H5_PINS
#define _H6_PINS
#define _H7_PINS

#define DIO_PIN(P) TERN(TARGET_LPC1768, P, analogInputToDigitalPin(P))

//
// Dual X, Dual Y, Multi-Z
// Chip Select and Digital Micro-stepping
//

#if HAS_X2_STEPPER
  #if PIN_EXISTS(X2_CS) && AXIS_HAS_SPI(X2)
    #define _X2_CS X2_CS_PIN,
  #else
    #define _X2_CS
  #endif
  #if PIN_EXISTS(X2_MS1)
    #define _X2_MS1 X2_MS1_PIN,
  #else
    #define _X2_MS1
  #endif
  #if PIN_EXISTS(X2_MS2)
    #define _X2_MS2 X2_MS2_PIN,
  #else
    #define _X2_MS2
  #endif
  #if PIN_EXISTS(X2_MS3)
    #define _X2_MS3 X2_MS3_PIN,
  #else
    #define _X2_MS3
  #endif
  #define _X2_PINS X2_STEP_PIN, X2_DIR_PIN, X2_ENABLE_PIN, _X2_CS _X2_MS1 _X2_MS2 _X2_MS3
#else
  #define _X2_PINS
#endif

#if ENABLED(Y_DUAL_STEPPER_DRIVERS)
  #if PIN_EXISTS(Y2_CS) && AXIS_HAS_SPI(Y2)
    #define _Y2_CS Y2_CS_PIN,
  #else
    #define _Y2_CS
  #endif
  #if PIN_EXISTS(Y2_MS1)
    #define _Y2_MS1 Y2_MS1_PIN,
  #else
    #define _Y2_MS1
  #endif
  #if PIN_EXISTS(Y2_MS2)
    #define _Y2_MS2 Y2_MS2_PIN,
  #else
    #define _Y2_MS2
  #endif
  #if PIN_EXISTS(Y2_MS3)
    #define _Y2_MS3 Y2_MS3_PIN,
  #else
    #define _Y2_MS3
  #endif
  #define _Y2_PINS Y2_STEP_PIN, Y2_DIR_PIN, Y2_ENABLE_PIN, _Y2_CS _Y2_MS1 _Y2_MS2 _Y2_MS3
#else
  #define _Y2_PINS
#endif

#if NUM_Z_STEPPER_DRIVERS >= 2
  #if PIN_EXISTS(Z2_CS) && AXIS_HAS_SPI(Z2)
    #define _Z2_CS Z2_CS_PIN,
  #else
    #define _Z2_CS
  #endif
  #if PIN_EXISTS(Z2_MS1)
    #define _Z2_MS1 Z2_MS1_PIN,
  #else
    #define _Z2_MS1
  #endif
  #if PIN_EXISTS(Z2_MS2)
    #define _Z2_MS2 Z2_MS2_PIN,
  #else
    #define _Z2_MS2
  #endif
  #if PIN_EXISTS(Z2_MS3)
    #define _Z2_MS3 Z2_MS3_PIN,
  #else
    #define _Z2_MS3
  #endif
  #define _Z2_PINS Z2_STEP_PIN, Z2_DIR_PIN, Z2_ENABLE_PIN, _Z2_CS _Z2_MS1 _Z2_MS2 _Z2_MS3
#else
  #define _Z2_PINS
#endif

#if NUM_Z_STEPPER_DRIVERS >= 3
  #if PIN_EXISTS(Z3_CS) && AXIS_HAS_SPI(Z3)
    #define _Z3_CS Z3_CS_PIN,
  #else
    #define _Z3_CS
  #endif
  #if PIN_EXISTS(Z3_MS1)
    #define _Z3_MS1 Z3_MS1_PIN,
  #else
    #define _Z3_MS1
  #endif
  #if PIN_EXISTS(Z3_MS2)
    #define _Z3_MS2 Z3_MS2_PIN,
  #else
    #define _Z3_MS2
  #endif
  #if PIN_EXISTS(Z3_MS3)
    #define _Z3_MS3 Z3_MS3_PIN,
  #else
    #define _Z3_MS3
  #endif
  #define _Z3_PINS Z3_STEP_PIN, Z3_DIR_PIN, Z3_ENABLE_PIN, _Z3_CS _Z3_MS1 _Z3_MS2 _Z3_MS3
#else
  #define _Z3_PINS
#endif

#if NUM_Z_STEPPER_DRIVERS >= 4
  #if PIN_EXISTS(Z4_CS) && AXIS_HAS_SPI(Z4)
    #define _Z4_CS Z4_CS_PIN,
  #else
    #define _Z4_CS
  #endif
  #if PIN_EXISTS(Z4_MS1)
    #define _Z4_MS1 Z4_MS1_PIN,
  #else
    #define _Z4_MS1
  #endif
  #if PIN_EXISTS(Z4_MS2)
    #define _Z4_MS2 Z4_MS2_PIN,
  #else
    #define _Z4_MS2
  #endif
  #if PIN_EXISTS(Z4_MS3)
    #define _Z4_MS3 Z4_MS3_PIN,
  #else
    #define _Z4_MS3
  #endif
  #define _Z4_PINS Z4_STEP_PIN, Z4_DIR_PIN, Z4_ENABLE_PIN, _Z4_CS _Z4_MS1 _Z4_MS2 _Z4_MS3
#else
  #define _Z4_PINS
#endif

//
// Generate the final Sensitive Pins array,
// keeping the array as small as possible.
//

#if PIN_EXISTS(PS_ON)
  #define _PS_ON PS_ON_PIN,
#else
  #define _PS_ON
#endif

#if HAS_BED_PROBE && PIN_EXISTS(Z_MIN_PROBE)
  #define _Z_PROBE Z_MIN_PROBE_PIN,
#else
  #define _Z_PROBE
#endif

#if PIN_EXISTS(FAN)
  #define _FAN0 FAN_PIN,
#else
  #define _FAN0
#endif
#if PIN_EXISTS(FAN1)
  #define _FAN1 FAN1_PIN,
#else
  #define _FAN1
#endif
#if PIN_EXISTS(FAN2)
  #define _FAN2 FAN2_PIN,
#else
  #define _FAN2
#endif
#if PIN_EXISTS(FAN3)
  #define _FAN3 FAN3_PIN,
#else
  #define _FAN3
#endif
#if PIN_EXISTS(FAN4)
  #define _FAN4 FAN4_PIN,
#else
  #define _FAN4
#endif
#if PIN_EXISTS(FAN5)
  #define _FAN5 FAN5_PIN,
#else
  #define _FAN5
#endif
#if PIN_EXISTS(FAN6)
  #define _FAN6 FAN6_PIN,
#else
  #define _FAN6
#endif
#if PIN_EXISTS(FAN7)
  #define _FAN7 FAN7_PIN,
#else
  #define _FAN7
#endif
#if PIN_EXISTS(CONTROLLER_FAN)
  #define _FANC CONTROLLER_FAN_PIN,
#else
  #define _FANC
#endif

#if TEMP_SENSOR_BED && PINS_EXIST(TEMP_BED, HEATER_BED)
  #define _BED_PINS HEATER_BED_PIN, DIO_PIN(TEMP_BED_PIN),
#else
  #define _BED_PINS
#endif

#if TEMP_SENSOR_CHAMBER && PIN_EXISTS(TEMP_CHAMBER)
  #define _CHAMBER_TEMP DIO_PIN(TEMP_CHAMBER_PIN),
#else
  #define _CHAMBER_TEMP
#endif
#if TEMP_SENSOR_CHAMBER && PINS_EXIST(TEMP_CHAMBER, HEATER_CHAMBER)
  #define _CHAMBER_HEATER HEATER_CHAMBER_PIN,
#else
  #define _CHAMBER_HEATER
#endif
#if TEMP_SENSOR_CHAMBER && PINS_EXIST(TEMP_CHAMBER, CHAMBER_AUTO_FAN)
  #define _CHAMBER_FAN CHAMBER_AUTO_FAN_PIN,
#else
  #define _CHAMBER_FAN
#endif

#if TEMP_SENSOR_COOLER && PIN_EXISTS(TEMP_COOLER)
  #define _COOLER_TEMP DIO_PIN(TEMP_COOLER_PIN),
#else
  #define _COOLER_TEMP
#endif
#if TEMP_SENSOR_COOLER && PIN_EXISTS(COOLER)
  #define _COOLER COOLER_PIN,
#else
  #define _COOLER
#endif
#if TEMP_SENSOR_COOLER && PINS_EXIST(TEMP_COOLER, COOLER_AUTO_FAN)
  #define _COOLER_FAN COOLER_AUTO_FAN_PIN,
#else
  #define _COOLER_FAN
#endif

#ifndef HAL_SENSITIVE_PINS
  #define HAL_SENSITIVE_PINS
#endif

#ifdef RUNTIME_ONLY_ANALOG_TO_DIGITAL
  #define _SP_END
#else
  #define _SP_END -2

  // Move a regular pin in front to the end
  template<pin_t F, pin_t ...D>
  struct OnlyPins : OnlyPins<D..., F> { };

  // Remove a -1 from the front
  template<pin_t ...D>
  struct OnlyPins<-1, D...> : OnlyPins<D...> { };

  // Remove -2 from the front, emit the rest, cease propagation
  template<pin_t ...D>
  struct OnlyPins<_SP_END, D...> { static constexpr size_t size = sizeof...(D); static constexpr pin_t table[sizeof...(D)] PROGMEM = { D... }; };
#endif

#define SENSITIVE_PINS \
  _X_PINS _Y_PINS _Z_PINS _I_PINS _J_PINS _K_PINS \
  _X2_PINS _Y2_PINS _Z2_PINS _Z3_PINS _Z4_PINS _Z_PROBE \
  _E0_PINS _E1_PINS _E2_PINS _E3_PINS _E4_PINS _E5_PINS _E6_PINS _E7_PINS \
  _H0_PINS _H1_PINS _H2_PINS _H3_PINS _H4_PINS _H5_PINS _H6_PINS _H7_PINS \
  _PS_ON _FAN0 _FAN1 _FAN2 _FAN3 _FAN4 _FAN5 _FAN6 _FAN7 _FANC \
  _BED_PINS _CHAMBER_TEMP _CHAMBER_HEATER _CHAMBER_FAN \
  _COOLER_TEMP _COOLER _COOLER_FAN HAL_SENSITIVE_PINS \
  _SP_END
