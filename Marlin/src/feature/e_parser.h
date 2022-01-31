/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#pragma once

/**
 * e_parser.h - Intercept special commands directly in the serial stream
 */

#include "../inc/MarlinConfigPre.h"

#if ENABLED(HOST_PROMPT_SUPPORT)
  #include "host_actions.h"
#endif

// External references
extern bool wait_for_user, wait_for_heatup;
extern int16_t feedrate_percentage;

#if ENABLED(REALTIME_COMMANDS)
  // From motion.h, which cannot be included here
  void report_current_position_moving();
  void quickpause_stepper();
  void quickresume_stepper();
#endif

#if ENABLED(REALTIME_FEEDRATE_CHANGES)
  // From motion.h, which cannot be included here
  // static float saved_feedrate_mm_s;
  // static int16_t saved_feedrate_percentage;
  // static int16_t backup_feedrate_percentage = 100;
  void restore_feedrate_and_scaling();
  void remember_feedrate_and_scaling();
#endif

void HAL_reboot();

class EmergencyParser {
 public:
  // Currently looking for: M108, M112, M220, M410, M876 S[0-9], S000, P000, R000
  enum State : uint8_t {
    EP_RESET,
    EP_N,
    EP_M,
    EP_M1,
    EP_M10,
    EP_M108,
    EP_M11,
    EP_M112,
  #if ENABLED(REALTIME_FEEDRATE_CHANGES)
    EP_M2,
    EP_M22,
    EP_M220,
    EP_M220S,
    EP_M220SN,
    EP_M220SNN,
    EP_M220SNNN,
  #endif
    EP_M4,
    EP_M41,
    EP_M410,
  #if ENABLED(HOST_PROMPT_SUPPORT)
    EP_M8,
    EP_M87,
    EP_M876,
    EP_M876S,
    EP_M876SN,
  #endif
  #if ENABLED(REALTIME_COMMANDS)
    EP_S,
    EP_S0,
    EP_S00,
    EP_GRBL_STATUS,
    EP_R,
    EP_R0,
    EP_R00,
    EP_GRBL_RESUME,
    EP_P,
    EP_P0,
    EP_P00,
    EP_GRBL_PAUSE,
  #endif
  #if ENABLED(SOFT_RESET_VIA_SERIAL)
    EP_ctrl,
    EP_K,
    EP_KI,
    EP_KIL,
    EP_KILL,
  #endif
    EP_IGNORE // to '\n'
  };

  static bool killed_by_M112;
  static bool quickstop_by_M410;
#if ENABLED(REALTIME_FEEDRATE_CHANGES)
  static int16_t M220_rate;
#endif

#if ENABLED(HOST_PROMPT_SUPPORT)
  static uint8_t M876_reason;
#endif

  EmergencyParser() { enable(); }

  FORCE_INLINE static void enable() { enabled = true; }
  FORCE_INLINE static void disable() { enabled = false; }

  FORCE_INLINE static void update(State &state, const uint8_t c) {
    switch (state) {
      case EP_RESET:
        switch (c) {
          case ' ':
          case '\n':
          case '\r':
            break;
          case 'N':
            state = EP_N;
            break;
          case 'M':
            state = EP_M;
            break;
#if ENABLED(REALTIME_COMMANDS)
          case 'S':
            state = EP_S;
            break;
          case 'P':
            state = EP_P;
            break;
          case 'R':
            state = EP_R;
            break;
#endif
#if ENABLED(SOFT_RESET_VIA_SERIAL)
          case '^':
            state = EP_ctrl;
            break;
          case 'K':
            state = EP_K;
            break;
#endif
          default:
            state = EP_IGNORE;
        }
        break;

      case EP_N:
        switch (c) {
          case '0' ... '9':
          case '-':
          case ' ':
            break;
          case 'M':
            state = EP_M;
            break;
#if ENABLED(REALTIME_COMMANDS)
          case 'S':
            state = EP_S;
            break;
          case 'P':
            state = EP_P;
            break;
          case 'R':
            state = EP_R;
            break;
#endif
          default:
            state = EP_IGNORE;
        }
        break;

#if ENABLED(REALTIME_COMMANDS)
      case EP_S:
        state = (c == '0') ? EP_S0 : EP_IGNORE;
        break;
      case EP_S0:
        state = (c == '0') ? EP_S00 : EP_IGNORE;
        break;
      case EP_S00:
        state = (c == '0') ? EP_GRBL_STATUS : EP_IGNORE;
        break;

      case EP_R:
        state = (c == '0') ? EP_R0 : EP_IGNORE;
        break;
      case EP_R0:
        state = (c == '0') ? EP_R00 : EP_IGNORE;
        break;
      case EP_R00:
        state = (c == '0') ? EP_GRBL_RESUME : EP_IGNORE;
        break;

      case EP_P:
        state = (c == '0') ? EP_P0 : EP_IGNORE;
        break;
      case EP_P0:
        state = (c == '0') ? EP_P00 : EP_IGNORE;
        break;
      case EP_P00:
        state = (c == '0') ? EP_GRBL_PAUSE : EP_IGNORE;
        break;
#endif

#if ENABLED(SOFT_RESET_VIA_SERIAL)
      case EP_ctrl:
        state = (c == 'X') ? EP_KILL : EP_IGNORE;
        break;
      case EP_K:
        state = (c == 'I') ? EP_KI : EP_IGNORE;
        break;
      case EP_KI:
        state = (c == 'L') ? EP_KIL : EP_IGNORE;
        break;
      case EP_KIL:
        state = (c == 'L') ? EP_KILL : EP_IGNORE;
        break;
#endif

      case EP_M:
        switch (c) {
          case ' ':
            break;
          case '1':
            state = EP_M1;
            break;
#if ENABLED(REALTIME_FEEDRATE_CHANGES)
          case '2':
            state = EP_M2;
            break;
#endif
          case '4':
            state = EP_M4;
            break;
#if ENABLED(HOST_PROMPT_SUPPORT)
          case '8':
            state = EP_M8;
            break;
#endif
          default:
            state = EP_IGNORE;
        }
        break;

      case EP_M1:
        switch (c) {
          case '0':
            state = EP_M10;
            break;
          case '1':
            state = EP_M11;
            break;
          default:
            state = EP_IGNORE;
        }
        break;

#if ENABLED(REALTIME_FEEDRATE_CHANGES)
        // Feed rate
      case EP_M2:
        state = (c == '2') ? EP_M22 : EP_IGNORE;
        break;
      case EP_M22:
        state = (c == '0') ? EP_M220 : EP_IGNORE;
        break;
      // Process M220 (feed rate)
      case EP_M220:
        switch (c) {
          case ' ':
            break;
          case 'S':
            state = EP_M220S;
            break;
          case 'B':
            remember_feedrate_and_scaling();
            state = EP_IGNORE;
            break;
          case 'R':
            restore_feedrate_and_scaling();
            state = EP_IGNORE;
            break;
          default:
            state = EP_IGNORE;
            break;
        }
        break;

      case EP_M220S:
        switch (c) {
          case '0' ... '9':
            state     = EP_M220SN;
            M220_rate = int16_t(c - '0');
            break;
          default: state = EP_IGNORE;
        }
        break;

      case EP_M220SN:
        switch (c) {
          case '0' ... '9':
            state     = EP_M220SNN;
            M220_rate = int16_t(c - '0');
            break;
          default: state = EP_IGNORE;
        }
        break;

      case EP_M220SNN:
        switch (c) {
          case '0' ... '9':
            state     = EP_M220SNNN;
            M220_rate = int16_t((M220_rate * 10) + (c - '0'));
            break;
          default: state = EP_IGNORE;
        }
        break;

      case EP_M220SNNN:
        switch (c) {
          case '0' ... '9':
            state               = EP_M220SNNN;
            feedrate_percentage = int16_t((M220_rate * 10) + (c - '0'));
            break;
          default: state = EP_IGNORE;
        }
        break;
#endif

        // Resume
      case EP_M10:
        state = (c == '8') ? EP_M108 : EP_IGNORE;
        break;
        //  Estop
      case EP_M11:
        state = (c == '2') ? EP_M112 : EP_IGNORE;
        break;
        // Quickstop
      case EP_M4:
        state = (c == '1') ? EP_M41 : EP_IGNORE;
        break;
      case EP_M41:
        state = (c == '0') ? EP_M410 : EP_IGNORE;
        break;

        // Process M876 (host response)
#if ENABLED(HOST_PROMPT_SUPPORT)

      case EP_M8:
        state = (c == '7') ? EP_M87 : EP_IGNORE;
        break;
      case EP_M87:
        state = (c == '6') ? EP_M876 : EP_IGNORE;
        break;

      case EP_M876:
        switch (c) {
          case ' ':
            break;
          case 'S':
            state = EP_M876S;
            break;
          default:
            state = EP_IGNORE;
            break;
        }
        break;

      case EP_M876S:
        switch (c) {
          case ' ':
            break;
          case '0' ... '9':
            state       = EP_M876SN;
            M876_reason = uint8_t(c - '0');
            break;
        }
        break;

#endif // HOST_PROMPT_SUPPORT

      case EP_IGNORE:
        if (ISEOL(c)) state = EP_RESET;
        break;

      default:
        if (ISEOL(c)) {
          if (enabled) switch (state) {
              case EP_M108:
                wait_for_user = wait_for_heatup = false;
                break;
              case EP_M112:
                killed_by_M112 = true;
                break;
              case EP_M410:
                quickstop_by_M410 = true;
                break;
#if ENABLED(REALTIME_FEEDRATE_CHANGES)
              case EP_M220SN:
              case EP_M220SNN:
              case EP_M220SNNN:
                LIMIT(M220_rate, 10, 999);
                if (M220_rate != feedrate_percentage) {
                  feedrate_percentage = M220_rate;
                }
                break;
#endif
#if ENABLED(HOST_PROMPT_SUPPORT)
              case EP_M876SN:
                hostui.handle_response(M876_reason);
                break;
#endif
#if ENABLED(REALTIME_COMMANDS)
              case EP_GRBL_STATUS:
                report_current_position_moving();
                break;
              case EP_GRBL_PAUSE:
                quickpause_stepper();
                break;
              case EP_GRBL_RESUME:
                quickresume_stepper();
                break;
#endif
#if ENABLED(SOFT_RESET_VIA_SERIAL)
              case EP_KILL:
                HAL_reboot();
                break;
#endif
              default:
                break;
            }
          state = EP_RESET;
        }
    }
  }

 private:
  static bool enabled;
};

extern EmergencyParser emergency_parser;
