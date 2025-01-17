/**
 * Modern Vintage CNC Firmware
*/

#include "../../inc/mvCNCConfig.h"

#if ENABLED(DUET_SMART_EFFECTOR) && PIN_EXISTS(SMART_EFFECTOR_MOD)

#include "../gcode.h"
#include "../../HAL/shared/Delay.h"
#include "../parser.h"

/**
 * The mvCNC format for the M672 command is different than shown in the Duet Smart Effector
 * documentation https://duet3d.dozuki.com/Wiki/Smart_effector_and_carriage_adapters_for_delta_cnc
 *
 * To set custom sensitivity:
 *   Duet:   M672 S105:aaa:bbb
 *   mvCNC: M672 Saaa
 *
 *   (where aaa is the desired sensitivity and bbb is 255 - aaa).
 *
 * Revert sensitivity to factory settings:
 *   Duet:   M672 S105:131:131
 *   mvCNC: M672 R
 */

#define M672_PROGBYTE    105                // magic byte to start programming custom sensitivity
#define M672_ERASEBYTE   131                // magic byte to clear custom sensitivity

//
// Smart Effector byte send protocol:
//
//  0  0  1  0       ... always 0010
//  b7 b6 b5 b4 ~b4  ... hi bits, NOT last bit
//  b3 b2 b1 b0 ~b0  ... lo bits, NOT last bit
//
void M672_send(uint8_t b) {    // bit rate requirement: 1kHz +/- 30%
  LOOP_L_N(bits, 14) {
    switch (bits) {
      default: { OUT_WRITE(SMART_EFFECTOR_MOD_PIN, !!(b & 0x80)); b <<= 1; break; } // send bit, shift next into place
      case  7:
      case 12: { OUT_WRITE(SMART_EFFECTOR_MOD_PIN, !!(b & 0x80));          break; } // send bit. no shift
      case  8:
      case 13: { OUT_WRITE(SMART_EFFECTOR_MOD_PIN,  !(b & 0x80)); b <<= 1; break; } // send inverted previous bit
      case  0: case  1:                                   // 00
      case  3: { OUT_WRITE(SMART_EFFECTOR_MOD_PIN, LOW); break; }   // 0010
      case  2: { OUT_WRITE(SMART_EFFECTOR_MOD_PIN, HIGH); break; }  // 001
    }
    DELAY_US(1000);
  }
}

/**
 * M672 - Set/reset Duet Smart Effector sensitivity
 *
 *  One of these is required:
 *    S<sensitivity> - 0-255
 *    R              - Flag to reset sensitivity to default
 */
void GcodeSuite::M672() {
  if (parser.seen('R')) {
    M672_send(M672_ERASEBYTE);
    M672_send(M672_ERASEBYTE);
  }
  else if (parser.seenval('S')) {
    const int8_t M672_sensitivity = parser.value_byte();
    M672_send(M672_PROGBYTE);
    M672_send(M672_sensitivity);
    M672_send(255 - M672_sensitivity);
  }
  else {
    SERIAL_ECHO_MSG("!'S' or 'R' parameter required.");
    return;
  }

  OUT_WRITE(SMART_EFFECTOR_MOD_PIN, LOW);  // Keep Smart Effector in NORMAL mode
}

#endif // DUET_SMART_EFFECTOR && SMART_EFFECTOR_MOD_PIN
