/**
 * Modern Vintage CNC Firmware
*/
#pragma once

/**
 * module/servo.h
 */

#include "../inc/mvCNCConfig.h"
#include "../HAL/shared/servo.h"

#if HAS_SERVO_ANGLES

  #if ENABLED(SWITCHING_EXTRUDER)
    // Switching ATC tool can have 2 or 4 angles
    #if ATC_TOOLS > 3
      #define REQ_ANGLES 4
    #else
      #define REQ_ANGLES 2
    #endif
    constexpr uint16_t sase[] = SWITCHING_EXTRUDER_SERVO_ANGLES;
    static_assert(COUNT(sase) == REQ_ANGLES, "SWITCHING_EXTRUDER_SERVO_ANGLES needs " STRINGIFY(REQ_ANGLES) " angles.");
  #else
    constexpr uint16_t sase[4] = { 0 };
  #endif

  #if ENABLED(SWITCHING_NOZZLE)
    constexpr uint16_t sasn[] = SWITCHING_NOZZLE_SERVO_ANGLES;
    static_assert(COUNT(sasn) == 2, "SWITCHING_NOZZLE_SERVO_ANGLES needs 2 angles.");
  #else
    constexpr uint16_t sasn[2] = { 0 };
  #endif

  #ifdef Z_PROBE_SERVO_NR
    #if ENABLED(BLTOUCH)
      #include "../feature/bltouch.h"
      #undef Z_SERVO_ANGLES
      #define Z_SERVO_ANGLES { BLTOUCH_DEPLOY, BLTOUCH_STOW }
    #endif
    constexpr uint16_t sazp[] = Z_SERVO_ANGLES;
    static_assert(COUNT(sazp) == 2, "Z_SERVO_ANGLES needs 2 angles.");
  #else
    constexpr uint16_t sazp[2] = { 0 };
  #endif

  #ifndef SWITCHING_EXTRUDER_SERVO_NR
    #define SWITCHING_EXTRUDER_SERVO_NR -1
  #endif
  #ifndef SWITCHING_EXTRUDER_E23_SERVO_NR
    #define SWITCHING_EXTRUDER_E23_SERVO_NR -1
  #endif
  #ifndef SWITCHING_NOZZLE_SERVO_NR
    #define SWITCHING_NOZZLE_SERVO_NR -1
  #endif
  #ifndef Z_PROBE_SERVO_NR
    #define Z_PROBE_SERVO_NR -1
  #endif

  #define ASRC(N,I) (                                  \
      N == SWITCHING_EXTRUDER_SERVO_NR     ? sase[I]   \
    : N == SWITCHING_EXTRUDER_E23_SERVO_NR ? sase[I+2] \
    : N == SWITCHING_NOZZLE_SERVO_NR       ? sasn[I]   \
    : N == Z_PROBE_SERVO_NR                ? sazp[I]   \
    : 0                                                )

  #if ENABLED(EDITABLE_SERVO_ANGLES)
    extern uint16_t servo_angles[NUM_SERVOS][2];
    #define CONST_SERVO_ANGLES base_servo_angles
  #else
    #define CONST_SERVO_ANGLES servo_angles
  #endif

  constexpr uint16_t CONST_SERVO_ANGLES [NUM_SERVOS][2] = {
      { ASRC(0,0), ASRC(0,1) }
    #if NUM_SERVOS > 1
      , { ASRC(1,0), ASRC(1,1) }
      #if NUM_SERVOS > 2
        , { ASRC(2,0), ASRC(2,1) }
        #if NUM_SERVOS > 3
          , { ASRC(3,0), ASRC(3,1) }
        #endif
      #endif
    #endif
  };

  #if HAS_Z_SERVO_PROBE
    #define DEPLOY_Z_SERVO() MOVE_SERVO(Z_PROBE_SERVO_NR, servo_angles[Z_PROBE_SERVO_NR][0])
    #define STOW_Z_SERVO() MOVE_SERVO(Z_PROBE_SERVO_NR, servo_angles[Z_PROBE_SERVO_NR][1])
  #endif

#endif // HAS_SERVO_ANGLES

#define MOVE_SERVO(I, P) servo[I].move(P)
#define DETACH_SERVO(I) servo[I].detach()

extern HAL_SERVO_LIB servo[NUM_SERVOS];
void servo_init();
