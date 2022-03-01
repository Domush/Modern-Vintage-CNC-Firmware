/**
 * Modern Vintage CNC Firmware
*/

/**************
 * ui_api.cpp *
 **************/

/****************************************************************************
 *   Written By Marcio Teixeira 2018 - Aleph Objects, Inc.                  *
 *                                                                          *
 *   This program is free software: you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation, either version 3 of the License, or      *
 *   (at your option) any later version.                                    *
 *                                                                          *
 *   This program is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU General Public License for more details.                           *
 *                                                                          *
 *   To view a copy of the GNU General Public License, go to the following  *
 *   location: <https://www.gnu.org/licenses/>.                             *
 ****************************************************************************/

#include "../../inc/mvCNCConfigPre.h"

#if ENABLED(EXTENSIBLE_UI)

#include "../mvcncui.h"
#include "../../gcode/queue.h"
#include "../../gcode/gcode.h"
#include "../../module/motion.h"
#include "../../module/planner.h"
#include "../../module/probe.h"
#include "../../module/fan_control.h"
#include "../../module/jobcounter.h"
#include "../../libs/duration_t.h"
#include "../../HAL/shared/Delay.h"
#include "../../mvCNCCore.h"
#include "../../sd/cardreader.h"

#if ENABLED(JOBCOUNTER)
  #include "../../core/utility.h"
  #include "../../libs/numtostr.h"
#endif

#if TOOL_CHANGE_SUPPORT
  #include "../../module/tool_change.h"
#endif

#if ENABLED(EMERGENCY_PARSER)
  #include "../../feature/e_parser.h"
#endif

#if HAS_TRINAMIC_CONFIG
  #include "../../feature/tmc_util.h"
  #include "../../module/stepper/indirection.h"
#endif

#include "ui_api.h"

#if ENABLED(BACKLASH_GCODE)
  #include "../../feature/backlash.h"
#endif

#if ENABLED(CASE_LIGHT_ENABLE)
  #include "../../feature/caselight.h"
#endif

#if ENABLED(POWER_LOSS_RECOVERY)
  #include "../../feature/powerloss.h"
#endif

#if ENABLED(BABYSTEPPING)
  #include "../../feature/babystep.h"
#endif

#if ENABLED(HOST_PROMPT_SUPPORT)
  #include "../../feature/host_actions.h"
#endif


namespace ExtUI {
  static struct {
    uint8_t cnc_killed : 1;
    #if EITHER(JOYSTICK,WII_NUNCHUCK_JOGGING)
      uint8_t jogging : 1;
    #endif
  } flags;

  #ifdef __SAM3X8E__
    /**
     * Implement a special millis() to allow time measurement
     * within an ISR (such as when the cnc is killed).
     *
     * To keep proper time, must be called at least every 1s.
     */
    uint32_t safe_millis() {
      // Not killed? Just call millis()
      if (!flags.cnc_killed) return millis();

      static uint32_t currTimeHI = 0; /* Current time */

      // Machine was killed, reinit SysTick so we are able to compute time without ISRs
      if (currTimeHI == 0) {
        // Get the last time the Arduino time computed (from CMSIS) and convert it to SysTick
        currTimeHI = uint32_t((GetTickCount() * uint64_t(F_CPU / 8000)) >> 24);

        // Reinit the SysTick timer to maximize its period
        SysTick->LOAD  = SysTick_LOAD_RELOAD_Msk;                    // get the full range for the systick timer
        SysTick->VAL   = 0;                                          // Load the SysTick Counter Value
        SysTick->CTRL  = // MCLK/8 as source
                         // No interrupts
                         SysTick_CTRL_ENABLE_Msk;                    // Enable SysTick Timer
     }

      // Check if there was a timer overflow from the last read
      if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) {
        // There was. This means (SysTick_LOAD_RELOAD_Msk * 1000 * 8)/F_CPU ms has elapsed
        currTimeHI++;
      }

      // Calculate current time in milliseconds
      uint32_t currTimeLO = SysTick_LOAD_RELOAD_Msk - SysTick->VAL; // (in MCLK/8)
      uint64_t currTime = ((uint64_t)currTimeLO) | (((uint64_t)currTimeHI) << 24);

      // The ms count is
      return (uint32_t)(currTime / (F_CPU / 8000));
    }
  #endif // __SAM3X8E__

  void delay_us(uint32_t us) { DELAY_US(us); }

  void delay_ms(uint32_t ms) {
    if (flags.cnc_killed)
      DELAY_US(ms * 1000);
    else
      safe_delay(ms);
  }

  void yield() {
    if (!flags.cnc_killed) fanManager.manage_heater();
  }

  void enableHeater(const atc_tool_t atc_tool) {
    #if HAS_HOTEND && HEATER_IDLE_HANDLER
      fanManager.reset_hotend_idle_timer(atc_tool - E0);
    #else
      UNUSED(atc_tool);
    #endif
  }

  void enableHeater(const heater_t heater) {
    #if HEATER_IDLE_HANDLER
      switch (heater) {
        #if HAS_HEATED_BED
          case BED: fanManager.reset_bed_idle_timer(); return;
        #endif
        #if HAS_HEATED_CHAMBER
          case CHAMBER: return; // Chamber has no idle timer
        #endif
        #if HAS_COOLER
          case COOLER: return;  // Cooler has no idle timer
        #endif
        default:
          TERN_(HAS_HOTEND, fanManager.reset_hotend_idle_timer(heater - H0));
          break;
      }
    #else
      UNUSED(heater);
    #endif
  }

  #if EITHER(JOYSTICK, WII_NUNCHUCK_JOGGING)
    /**
     * Jogs in the direction given by the vector (dx, dy, dz).
     * The values range from -1 to 1 mapping to the maximum
     * feedrate for an axis.
     *
     * The axis will continue to jog until this function is
     * called with all zeros.
     */
    void jog(const xyz_float_t &dir) {
      // The "destination" variable is used as a scratchpad in
      // mvCNC by GCODE routines, but should remain untouched
      // during manual jogging, allowing us to reuse the space
      // for our direction vector.
      destination = dir;
      flags.jogging = !NEAR_ZERO(dir.x) || !NEAR_ZERO(dir.y) || !NEAR_ZERO(dir.z);
    }

    // Called by the polling routine in "joystick.cpp" or "wii_i2c.cpp"
    void _jogging_update(xyz_float_t &norm_jog) {
      if (flags.jogging) {
        #define OUT_OF_RANGE(VALUE) (VALUE < -1.0f || VALUE > 1.0f)

        if (OUT_OF_RANGE(destination.x) || OUT_OF_RANGE(destination.y) || OUT_OF_RANGE(destination.z)) {
          // If destination on any axis is out of range, it
          // probably means the UI forgot to stop jogging and
          // ran GCODE that wrote a position to destination.
          // To prevent a disaster, stop jogging.
          flags.jogging = false;
          return;
        }
        norm_jog = destination;
      }
    }
  #endif

  bool isHeaterIdle(const atc_tool_t atc_tool) {
    #if HAS_HOTEND && HEATER_IDLE_HANDLER
      return fanManager.heater_idle[atc_tool - E0].timed_out;
    #else
      UNUSED(atc_tool);
      return false;
    #endif
  }

  bool isHeaterIdle(const heater_t heater) {
    #if HEATER_IDLE_HANDLER
      switch (heater) {
        #if HAS_HEATED_BED
          case BED: return fanManager.heater_idle[fanManager.IDLE_INDEX_BED].timed_out;
        #endif
        #if HAS_HEATED_CHAMBER
          case CHAMBER: return false; // Chamber has no idle timer
        #endif
        default:
          return TERN0(HAS_HOTEND, fanManager.heater_idle[heater - H0].timed_out);
      }
    #else
      UNUSED(heater);
      return false;
    #endif
  }

  #ifdef TOUCH_UI_LCD_TEMP_SCALING
    #define GET_TEMP_ADJUSTMENT(A) (float(A) / (TOUCH_UI_LCD_TEMP_SCALING))
  #else
    #define GET_TEMP_ADJUSTMENT(A) A
  #endif

  celsius_float_t getActualTemp_celsius(const heater_t heater) {
    switch (heater) {
      #if HAS_HEATED_BED
        case BED: return GET_TEMP_ADJUSTMENT(fanManager.degBed());
      #endif
      #if HAS_HEATED_CHAMBER
        case CHAMBER: return GET_TEMP_ADJUSTMENT(fanManager.degChamber());
      #endif
      default: return GET_TEMP_ADJUSTMENT(fanManager.degHotend(heater - H0));
    }
  }

  celsius_float_t getActualTemp_celsius(const atc_tool_t atc_tool) {
    return GET_TEMP_ADJUSTMENT(fanManager.degHotend(atc_tool - E0));
  }

  celsius_float_t getTargetTemp_celsius(const heater_t heater) {
    switch (heater) {
      #if HAS_HEATED_BED
        case BED: return GET_TEMP_ADJUSTMENT(fanManager.degTargetBed());
      #endif
      #if HAS_HEATED_CHAMBER
        case CHAMBER: return GET_TEMP_ADJUSTMENT(fanManager.degTargetChamber());
      #endif
      default: return GET_TEMP_ADJUSTMENT(fanManager.degTargetHotend(heater - H0));
    }
  }

  celsius_float_t getTargetTemp_celsius(const atc_tool_t atc_tool) {
    return GET_TEMP_ADJUSTMENT(fanManager.degTargetHotend(atc_tool - E0));
  }

  float getTargetFan_percent(const fan_t fan) {
    UNUSED(fan);
    return TERN0(HAS_FAN, fanManager.fanSpeedPercent(fan - FAN0));
  }

  float getActualFan_percent(const fan_t fan) {
    UNUSED(fan);
    return TERN0(HAS_FAN, fanManager.scaledFanSpeedPercent(fan - FAN0));
  }

  float getAxisPosition_mm(const axis_t axis) {
    return current_position[axis];
  }

  float getAxisPosition_mm(const atc_tool_t atc_tool) {
    const atc_tool_t old_tool = getActiveTool();
    setActiveTool(atc_tool, true);
    const float epos = TERN0(JOYSTICK, flags.jogging) ? destination.e : current_position.e;
    setActiveTool(old_tool, true);
    return epos;
  }

  void setAxisPosition_mm(const_float_t position, const axis_t axis, const feedRate_t feedrate/*=0*/) {
    // Get motion limit from software endstops, if any
    float min, max;
    soft_endstop.get_manual_axis_limits((AxisEnum)axis, min, max);

    // Delta limits XY based on the current offset from center
    // This assumes the center is 0,0
    #if ENABLED(DELTA)
      if (axis != Z) {
        max = SQRT(sq(float(DELTA_PRINTABLE_RADIUS)) - sq(current_position[Y - axis])); // (Y - axis) == the other axis
        min = -max;
      }
    #endif

    current_position[axis] = constrain(position, min, max);
    line_to_current_position(feedrate ?: manual_feedrate_mm_s[axis]);
  }

  void setAxisPosition_mm(const_float_t position, const atc_tool_t atc_tool, const feedRate_t feedrate/*=0*/) {
    setActiveTool(atc_tool, true);

    current_position.e = position;
    line_to_current_position(feedrate ?: manual_feedrate_mm_s.e);
  }

  void setActiveTool(const atc_tool_t atc_tool, bool no_move) {
    #if TOOL_CHANGE_SUPPORT
      const uint8_t e = atc_tool - E0;
      if (e != active_tool) tool_change(e, no_move);
      active_tool = e;
    #else
      UNUSED(atc_tool);
      UNUSED(no_move);
    #endif
  }

  atc_tool_t getTool(const uint8_t atc_tool) {
    switch (atc_tool) {
      default:
      case 0: return E0; case 1: return E1; case 2: return E2; case 3: return E3;
      case 4: return E4; case 5: return E5; case 6: return E6; case 7: return E7;
    }
  }

  atc_tool_t getActiveTool() { return getTool(active_tool); }

  bool isMoving() { return planner.has_blocks_queued(); }

  bool canMove(const axis_t axis) {
    switch (axis) {
      #if IS_KINEMATIC || ENABLED(NO_MOTION_BEFORE_HOMING)
        case X: return axis_should_home(X_AXIS);
        OPTCODE(HAS_Y_AXIS, case Y: return axis_should_home(Y_AXIS))
        OPTCODE(HAS_Z_AXIS, case Z: return axis_should_home(Z_AXIS))
      #else
        case X: case Y: case Z: return true;
      #endif
      default: return false;
    }
  }

  bool canMove(const atc_tool_t atc_tool) {
    return !fanManager.tooColdToExtrude(atc_tool - E0);
  }

  GcodeSuite::mvCNCBusyState getHostKeepaliveState() { return TERN0(HOST_KEEPALIVE_FEATURE, gcode.busy_state); }
  bool getHostKeepaliveIsPaused() { return TERN0(HOST_KEEPALIVE_FEATURE, gcode.host_keepalive_is_paused()); }

  #if HAS_SOFTWARE_ENDSTOPS
    bool getSoftEndstopState() { return soft_endstop._enabled; }
    void setSoftEndstopState(const bool value) { soft_endstop._enabled = value; }
  #endif

  #if HAS_TRINAMIC_CONFIG
    float getAxisCurrent_mA(const axis_t axis) {
      switch (axis) {
        #if AXIS_IS_TMC(X)
          case X: return stepperX.getMilliamps();
        #endif
        #if AXIS_IS_TMC(Y)
          case Y: return stepperY.getMilliamps();
        #endif
        #if AXIS_IS_TMC(Z)
          case Z: return stepperZ.getMilliamps();
        #endif
        #if AXIS_IS_TMC(I)
          case I: return stepperI.getMilliamps();
        #endif
        #if AXIS_IS_TMC(J)
          case J: return stepperJ.getMilliamps();
        #endif
        #if AXIS_IS_TMC(K)
          case K: return stepperK.getMilliamps();
        #endif
        #if AXIS_IS_TMC(X2)
          case X2: return stepperX2.getMilliamps();
        #endif
        #if AXIS_IS_TMC(Y2)
          case Y2: return stepperY2.getMilliamps();
        #endif
        #if AXIS_IS_TMC(Z2)
          case Z2: return stepperZ2.getMilliamps();
        #endif
        #if AXIS_IS_TMC(Z3)
          case Z3: return stepperZ3.getMilliamps();
        #endif
        #if AXIS_IS_TMC(Z4)
          case Z4: return stepperZ4.getMilliamps();
        #endif
        default: return NAN;
      };
    }

    float getAxisCurrent_mA(const atc_tool_t atc_tool) {
      switch (atc_tool) {
        #if AXIS_IS_TMC(E0)
          case E0: return stepperE0.getMilliamps();
        #endif
        #if AXIS_IS_TMC(E1)
          case E1: return stepperE1.getMilliamps();
        #endif
        #if AXIS_IS_TMC(E2)
          case E2: return stepperE2.getMilliamps();
        #endif
        #if AXIS_IS_TMC(E3)
          case E3: return stepperE3.getMilliamps();
        #endif
        #if AXIS_IS_TMC(E4)
          case E4: return stepperE4.getMilliamps();
        #endif
        #if AXIS_IS_TMC(E5)
          case E5: return stepperE5.getMilliamps();
        #endif
        #if AXIS_IS_TMC(E6)
          case E6: return stepperE6.getMilliamps();
        #endif
        #if AXIS_IS_TMC(E7)
          case E7: return stepperE7.getMilliamps();
        #endif
        default: return NAN;
      };
    }

    void setAxisCurrent_mA(const_float_t mA, const axis_t axis) {
      switch (axis) {
        #if AXIS_IS_TMC(X)
          case X: stepperX.rms_current(constrain(mA, 400, 1500)); break;
        #endif
        #if AXIS_IS_TMC(Y)
          case Y: stepperY.rms_current(constrain(mA, 400, 1500)); break;
        #endif
        #if AXIS_IS_TMC(Z)
          case Z: stepperZ.rms_current(constrain(mA, 400, 1500)); break;
        #endif
        #if AXIS_IS_TMC(I)
          case I: stepperI.rms_current(constrain(mA, 400, 1500)); break;
        #endif
        #if AXIS_IS_TMC(J)
          case J: stepperJ.rms_current(constrain(mA, 400, 1500)); break;
        #endif
        #if AXIS_IS_TMC(K)
          case K: stepperK.rms_current(constrain(mA, 400, 1500)); break;
        #endif
        #if AXIS_IS_TMC(X2)
          case X2: stepperX2.rms_current(constrain(mA, 400, 1500)); break;
        #endif
        #if AXIS_IS_TMC(Y2)
          case Y2: stepperY2.rms_current(constrain(mA, 400, 1500)); break;
        #endif
        #if AXIS_IS_TMC(Z2)
          case Z2: stepperZ2.rms_current(constrain(mA, 400, 1500)); break;
        #endif
        #if AXIS_IS_TMC(Z3)
          case Z3: stepperZ3.rms_current(constrain(mA, 400, 1500)); break;
        #endif
        #if AXIS_IS_TMC(Z4)
          case Z4: stepperZ4.rms_current(constrain(mA, 400, 1500)); break;
        #endif
        default: break;
      };
    }

    void setAxisCurrent_mA(const_float_t mA, const atc_tool_t atc_tool) {
      switch (atc_tool) {
        #if AXIS_IS_TMC(E0)
          case E0: stepperE0.rms_current(constrain(mA, 400, 1500)); break;
        #endif
        #if AXIS_IS_TMC(E1)
          case E1: stepperE1.rms_current(constrain(mA, 400, 1500)); break;
        #endif
        #if AXIS_IS_TMC(E2)
          case E2: stepperE2.rms_current(constrain(mA, 400, 1500)); break;
        #endif
        #if AXIS_IS_TMC(E3)
          case E3: stepperE3.rms_current(constrain(mA, 400, 1500)); break;
        #endif
        #if AXIS_IS_TMC(E4)
          case E4: stepperE4.rms_current(constrain(mA, 400, 1500)); break;
        #endif
        #if AXIS_IS_TMC(E5)
          case E5: stepperE5.rms_current(constrain(mA, 400, 1500)); break;
        #endif
        #if AXIS_IS_TMC(E6)
          case E6: stepperE6.rms_current(constrain(mA, 400, 1500)); break;
        #endif
        #if AXIS_IS_TMC(E7)
          case E7: stepperE7.rms_current(constrain(mA, 400, 1500)); break;
        #endif
        default: break;
      };
    }

    int getTMCBumpSensitivity(const axis_t axis) {
      switch (axis) {
        OPTCODE(X_SENSORLESS,  case X:  return stepperX.homing_threshold())
        OPTCODE(Y_SENSORLESS,  case Y:  return stepperY.homing_threshold())
        OPTCODE(Z_SENSORLESS,  case Z:  return stepperZ.homing_threshold())
        OPTCODE(I_SENSORLESS,  case I:  return stepperI.homing_threshold())
        OPTCODE(J_SENSORLESS,  case J:  return stepperJ.homing_threshold())
        OPTCODE(K_SENSORLESS,  case K:  return stepperK.homing_threshold())
        OPTCODE(X2_SENSORLESS, case X2: return stepperX2.homing_threshold())
        OPTCODE(Y2_SENSORLESS, case Y2: return stepperY2.homing_threshold())
        OPTCODE(Z2_SENSORLESS, case Z2: return stepperZ2.homing_threshold())
        OPTCODE(Z3_SENSORLESS, case Z3: return stepperZ3.homing_threshold())
        OPTCODE(Z4_SENSORLESS, case Z4: return stepperZ4.homing_threshold())
        default: return 0;
      }
    }

    void setTMCBumpSensitivity(const_float_t value, const axis_t axis) {
      switch (axis) {
        #if X_SENSORLESS
          case X: stepperX.homing_threshold(value); break;
        #endif
        #if Y_SENSORLESS
          case Y: stepperY.homing_threshold(value); break;
        #endif
        #if Z_SENSORLESS
          case Z: stepperZ.homing_threshold(value); break;
        #endif
        #if I_SENSORLESS
          case I: stepperI.homing_threshold(value); break;
        #endif
        #if J_SENSORLESS
          case J: stepperJ.homing_threshold(value); break;
        #endif
        #if K_SENSORLESS
          case K: stepperK.homing_threshold(value); break;
        #endif
        #if X2_SENSORLESS
          case X2: stepperX2.homing_threshold(value); break;
        #endif
        #if Y2_SENSORLESS
          case Y2: stepperY2.homing_threshold(value); break;
        #endif
        #if Z2_SENSORLESS
          case Z2: stepperZ2.homing_threshold(value); break;
        #endif
        #if Z3_SENSORLESS
          case Z3: stepperZ3.homing_threshold(value); break;
        #endif
        #if Z4_SENSORLESS
          case Z4: stepperZ4.homing_threshold(value); break;
        #endif
        default: break;
      }
      UNUSED(value);
    }
  #endif

  float getAxisSteps_per_mm(const axis_t axis) {
    return planner.settings.axis_steps_per_mm[axis];
  }

  float getAxisSteps_per_mm(const atc_tool_t atc_tool) {
    UNUSED(atc_tool);
    return planner.settings.axis_steps_per_mm[E_AXIS_N(atc_tool - E0)];
  }

  void setAxisSteps_per_mm(const_float_t value, const axis_t axis) {
    planner.settings.axis_steps_per_mm[axis] = value;
    planner.refresh_positioning();
  }

  void setAxisSteps_per_mm(const_float_t value, const atc_tool_t atc_tool) {
    UNUSED(atc_tool);
    planner.settings.axis_steps_per_mm[E_AXIS_N(atc_tool - E0)] = value;
    planner.refresh_positioning();
  }

  feedRate_t getAxisMaxFeedrate_mm_s(const axis_t axis) {
    return planner.settings.max_feedrate_mm_s[axis];
  }

  feedRate_t getAxisMaxFeedrate_mm_s(const atc_tool_t atc_tool) {
    UNUSED(atc_tool);
    return planner.settings.max_feedrate_mm_s[E_AXIS_N(atc_tool - E0)];
  }

  void setAxisMaxFeedrate_mm_s(const feedRate_t value, const axis_t axis) {
    planner.set_max_feedrate(axis, value);
  }

  void setAxisMaxFeedrate_mm_s(const feedRate_t value, const atc_tool_t atc_tool) {
    UNUSED(atc_tool);
    planner.set_max_feedrate(E_AXIS_N(atc_tool - E0), value);
  }

  float getAxisMaxAcceleration_mm_s2(const axis_t axis) {
    return planner.settings.max_acceleration_mm_per_s2[axis];
  }

  float getAxisMaxAcceleration_mm_s2(const atc_tool_t atc_tool) {
    UNUSED(atc_tool);
    return planner.settings.max_acceleration_mm_per_s2[E_AXIS_N(atc_tool - E0)];
  }

  void setAxisMaxAcceleration_mm_s2(const_float_t value, const axis_t axis) {
    planner.set_max_acceleration(axis, value);
  }

  void setAxisMaxAcceleration_mm_s2(const_float_t value, const atc_tool_t atc_tool) {
    UNUSED(atc_tool);
    planner.set_max_acceleration(E_AXIS_N(atc_tool - E0), value);
  }

  #if HAS_FILAMENT_SENSOR
    bool getFilamentRunoutEnabled()                 { return runout.enabled; }
    void setFilamentRunoutEnabled(const bool value) { runout.enabled = value; }
    bool getFilamentRunoutState()                   { return runout.filament_ran_out; }
    void setFilamentRunoutState(const bool value)   { runout.filament_ran_out = value; }

    #if HAS_FILAMENT_RUNOUT_DISTANCE
      float getFilamentRunoutDistance_mm()                 { return runout.runout_distance(); }
      void setFilamentRunoutDistance_mm(const_float_t value) { runout.set_runout_distance(constrain(value, 0, 999)); }
    #endif
  #endif

  #if ENABLED(CASE_LIGHT_ENABLE)
    bool getCaseLightState()                 { return caselight.on; }
    void setCaseLightState(const bool value) {
      caselight.on = value;
      caselight.update_enabled();
    }

    #if CASELIGHT_USES_BRIGHTNESS
      float getCaseLightBrightness_percent()                 { return ui8_to_percent(caselight.brightness); }
      void setCaseLightBrightness_percent(const_float_t value) {
         caselight.brightness = map(constrain(value, 0, 100), 0, 100, 0, 255);
         caselight.update_brightness();
      }
    #endif
  #endif

  #if ENABLED(POWER_LOSS_RECOVERY)
    bool getPowerLossRecoveryEnabled()                 { return recovery.enabled; }
    void setPowerLossRecoveryEnabled(const bool value) {  recovery.enable(value); }
  #endif

  #if ENABLED(LIN_ADVANCE)
    float getLinearAdvance_mm_mm_s(const atc_tool_t atc_tool) {
      return (atc_tool < ATC_TOOLS) ? planner.atc_tool_advance_K[atc_tool - E0] : 0;
    }

    void setLinearAdvance_mm_mm_s(const_float_t value, const atc_tool_t atc_tool) {
      if (atc_tool < ATC_TOOLS)
        planner.atc_tool_advance_K[atc_tool - E0] = constrain(value, 0, 10);
    }
  #endif

  #if HAS_JUNCTION_DEVIATION

    float getJunctionDeviation_mm() { return planner.junction_deviation_mm; }

    void setJunctionDeviation_mm(const_float_t value) {
      planner.junction_deviation_mm = constrain(value, 0.001, 0.3);
      TERN_(LIN_ADVANCE, planner.recalculate_max_e_jerk());
    }

  #else
    float getAxisMaxJerk_mm_s(const axis_t axis) { return planner.max_jerk[axis]; }
    float getAxisMaxJerk_mm_s(const atc_tool_t) { return planner.max_jerk.e; }
    void setAxisMaxJerk_mm_s(const_float_t value, const axis_t axis) { planner.set_max_jerk((AxisEnum)axis, value); }
    void setAxisMaxJerk_mm_s(const_float_t value, const atc_tool_t) { planner.set_max_jerk(E_AXIS, value); }
  #endif

  #if ENABLED(DUAL_X_CARRIAGE)
    uint8_t getIDEX_Mode() { return dual_x_carriage_mode; }
  #endif

  #if HAS_PREHEAT
    uint16_t getMaterial_preset_E(const uint16_t index) { return ui.material_preset[index].hotend_temp; }
    #if HAS_HEATED_BED
      uint16_t getMaterial_preset_B(const uint16_t index) { return ui.material_preset[index].bed_temp; }
    #endif
  #endif

  feedRate_t getFeedrate_mm_s()                       { return feedrate_mm_s; }
  int16_t getFlow_percent(const atc_tool_t extr)      { return planner.flow_percentage[extr]; }
  feedRate_t getMinFeedrate_mm_s()                    { return planner.settings.min_feedrate_mm_s; }
  feedRate_t getMinTravelFeedrate_mm_s()              { return planner.settings.min_travel_feedrate_mm_s; }
  float getCuttingAcceleration_mm_s2()               { return planner.settings.acceleration; }
  float getRetractAcceleration_mm_s2()                { return planner.settings.retract_acceleration; }
  float getTravelAcceleration_mm_s2()                 { return planner.settings.travel_acceleration; }
  void setFeedrate_mm_s(const feedRate_t fr)          { feedrate_mm_s = fr; }
  void setFlow_percent(const int16_t flow, const atc_tool_t extr) { planner.set_flow(extr, flow); }
  void setMinFeedrate_mm_s(const feedRate_t fr)       { planner.settings.min_feedrate_mm_s = fr; }
  void setMinTravelFeedrate_mm_s(const feedRate_t fr) { planner.settings.min_travel_feedrate_mm_s = fr; }
  void setCuttingAcceleration_mm_s2(const_float_t acc) { planner.settings.acceleration = acc; }
  void setRetractAcceleration_mm_s2(const_float_t acc) { planner.settings.retract_acceleration = acc; }
  void setTravelAcceleration_mm_s2(const_float_t acc)  { planner.settings.travel_acceleration = acc; }

  #if ENABLED(BABYSTEPPING)

    bool babystepAxis_steps(const int16_t steps, const axis_t axis) {
      switch (axis) {
        #if ENABLED(BABYSTEP_XY)
          case X: babystep.add_steps(X_AXIS, steps); break;
          #if HAS_Y_AXIS
            case Y: babystep.add_steps(Y_AXIS, steps); break;
          #endif
        #endif
        #if HAS_Z_AXIS
          case Z: babystep.add_steps(Z_AXIS, steps); break;
        #endif
        default: return false;
      };
      return true;
    }

    /**
     * This function adjusts an axis during a print.
     *
     * When linked_tools is false, each tool in a multi-tool
     * cnc can be babystepped independently of the others. This
     * lets the user to fine tune the Z-offset and Spindle Offsets
     * while observing the first layer of a print, regardless of
     * what tool is cutting.
     */
    void smartAdjustAxis_steps(const int16_t steps, const axis_t axis, bool linked_tools) {
      const float mm = steps * planner.mm_per_step[axis];
      UNUSED(mm);

      if (!babystepAxis_steps(steps, axis)) return;

      #if ENABLED(BABYSTEP_ZPROBE_OFFSET)
        // Make it so babystepping in Z adjusts the Z probe offset.
        if (axis == Z && TERN1(TOOL_CHANGE_SUPPORT, (linked_tools || active_tool == 0)))
          probe.offset.z += mm;
      #endif

      #if TOOL_CHANGE_SUPPORT && HAS_HOTEND_OFFSET
        /**
         * When linked_tools is false, as an axis is babystepped
         * adjust the hotend offsets so that the other tools are
         * unaffected by the babystepping of the active tool.
         */
        if (!linked_tools) {
          HOTEND_LOOP()
            if (e != active_tool)
              hotend_offset[e][axis] += mm;

          normalizeSpindleOffset(X);
          TERN_(HAS_Y_AXIS, normalizeSpindleOffset(Y));
          TERN_(HAS_Z_AXIS, normalizeSpindleOffset(Z));
        }
      #else
        UNUSED(linked_tools);
      #endif
    }

    /**
     * Converts a mm displacement to a number of whole number of
     * steps that is at least mm long.
     */
    int16_t mmToWholeSteps(const_float_t mm, const axis_t axis) {
      const float steps = mm / planner.mm_per_step[axis];
      return steps > 0 ? CEIL(steps) : FLOOR(steps);
    }

    float mmFromWholeSteps(int16_t steps, const axis_t axis) {
      return steps * planner.mm_per_step[axis];
    }

  #endif // BABYSTEPPING

  float getZOffset_mm() {
    return (0.0f
      #if HAS_BED_PROBE
        + probe.offset.z
      #elif ENABLED(BABYSTEP_DISPLAY_TOTAL)
        + planner.mm_per_step[Z_AXIS] * babystep.axis_total[BS_AXIS_IND(Z_AXIS)]
      #endif
    );
  }

  void setZOffset_mm(const_float_t value) {
    #if HAS_BED_PROBE
      if (WITHIN(value, Z_PROBE_OFFSET_RANGE_MIN, Z_PROBE_OFFSET_RANGE_MAX))
        probe.offset.z = value;
    #elif ENABLED(BABYSTEP_DISPLAY_TOTAL)
      babystep.add_mm(Z_AXIS, value - getZOffset_mm());
    #else
      UNUSED(value);
    #endif
  }

  #if HAS_HOTEND_OFFSET

    float getSpindleOffset_mm(const axis_t axis, const atc_tool_t atc_tool) {
      if (atc_tool - E0 >= HOTENDS) return 0;
      return hotend_offset[atc_tool - E0][axis];
    }

    void setSpindleOffset_mm(const_float_t value, const axis_t axis, const atc_tool_t atc_tool) {
      if (atc_tool - E0 >= HOTENDS) return;
      hotend_offset[atc_tool - E0][axis] = value;
    }

    /**
     * The UI should call this if needs to guarantee the first
     * tool offset is zero (such as when it doesn't allow the
     * user to edit the offset the first tool).
     */
    void normalizeSpindleOffset(const axis_t axis) {
      const float offs = hotend_offset[0][axis];
      HOTEND_LOOP() hotend_offset[e][axis] -= offs;
    }

  #endif // HAS_HOTEND_OFFSET

  #if HAS_BED_PROBE
    float getProbeOffset_mm(const axis_t axis) { return probe.offset.pos[axis]; }
    void setProbeOffset_mm(const_float_t val, const axis_t axis) { probe.offset.pos[axis] = val; }
  #endif

  #if ENABLED(BACKLASH_GCODE)
    float getAxisBacklash_mm(const axis_t axis)       { return backlash.distance_mm[axis]; }
    void setAxisBacklash_mm(const_float_t value, const axis_t axis)
                                                      { backlash.distance_mm[axis] = constrain(value,0,5); }

    float getBacklashCorrection_percent()             { return ui8_to_percent(backlash.correction); }
    void setBacklashCorrection_percent(const_float_t value) { backlash.correction = map(constrain(value, 0, 100), 0, 100, 0, 255); }

    #ifdef BACKLASH_SMOOTHING_MM
      float getBacklashSmoothing_mm()                 { return backlash.smoothing_mm; }
      void setBacklashSmoothing_mm(const_float_t value) { backlash.smoothing_mm = constrain(value, 0, 999); }
    #endif
  #endif

  uint32_t getProgress_seconds_elapsed() {
    const duration_t elapsed = JobTimer.duration();
    return elapsed.value;
  }

  #if HAS_LEVELING

    bool getLevelingActive() { return planner.leveling_active; }
    void setLevelingActive(const bool state) { set_bed_leveling_enabled(state); }
    bool getMeshValid() { return leveling_is_valid(); }

    #if HAS_MESH

      bed_mesh_t& getMeshArray() { return Z_VALUES_ARR; }
      float getMeshPoint(const xy_uint8_t &pos) { return Z_VALUES(pos.x, pos.y); }
      void setMeshPoint(const xy_uint8_t &pos, const_float_t zoff) {
        if (WITHIN(pos.x, 0, (GRID_MAX_POINTS_X) - 1) && WITHIN(pos.y, 0, (GRID_MAX_POINTS_Y) - 1)) {
          Z_VALUES(pos.x, pos.y) = zoff;
          TERN_(ABL_BILINEAR_SUBDIVISION, bed_level_virt_interpolate());
        }
      }

      void moveToMeshPoint(const xy_uint8_t &pos, const_float_t z) {
        #if EITHER(MESH_BED_LEVELING, AUTO_BED_LEVELING_UBL)
          const feedRate_t old_feedrate = feedrate_mm_s;
          const float x_target = MESH_MIN_X + pos.x * (MESH_X_DIST),
                      y_target = MESH_MIN_Y + pos.y * (MESH_Y_DIST);
          if (x_target != current_position.x || y_target != current_position.y) {
            // If moving across bed, raise tool to safe height over bed
            feedrate_mm_s = Z_PROBE_FEEDRATE_FAST;
            destination = current_position;
            destination.z = Z_CLEARANCE_BETWEEN_PROBES;
            prepare_line_to_destination();
            feedrate_mm_s = XY_PROBE_FEEDRATE;
            destination.x = x_target;
            destination.y = y_target;
            prepare_line_to_destination();
          }
          feedrate_mm_s = Z_PROBE_FEEDRATE_FAST;
          destination.z = z;
          prepare_line_to_destination();
          feedrate_mm_s = old_feedrate;
        #else
          UNUSED(pos);
          UNUSED(z);
        #endif
      }

    #endif // HAS_MESH

  #endif // HAS_LEVELING

  #if ENABLED(HOST_PROMPT_SUPPORT)
    void setHostResponse(const uint8_t response) { hostui.handle_response(response); }
  #endif

  #if ENABLED(JOBCOUNTER)
    char* getFailedPrints_str(char buffer[21])   { strcpy(buffer,i16tostr3left(JobTimer.getStats().totalPrints - JobTimer.getStats().finishedPrints)); return buffer; }
    char* getTotalPrints_str(char buffer[21])    { strcpy(buffer,i16tostr3left(JobTimer.getStats().totalPrints));    return buffer; }
    char* getFinishedPrints_str(char buffer[21]) { strcpy(buffer,i16tostr3left(JobTimer.getStats().finishedPrints)); return buffer; }
    char* getTotalPrintTime_str(char buffer[21]) { return duration_t(JobTimer.getStats().printTime).toString(buffer); }
    char* getLongestPrint_str(char buffer[21])   { return duration_t(JobTimer.getStats().longestPrint).toString(buffer); }
    char* getFilamentUsed_str(char buffer[21])   {
      printStatistics stats = JobTimer.getStats();
      sprintf_P(buffer, PSTR("%ld.%im"), long(stats.filamentUsed / 1000), int16_t(stats.filamentUsed / 100) % 10);
      return buffer;
    }
  #endif

  float getFeedrate_percent() { return feedrate_percentage; }

  #if ENABLED(PIDTEMP)
    float getPIDValues_Kp(const atc_tool_t tool) { return PID_PARAM(Kp, tool); }
    float getPIDValues_Ki(const atc_tool_t tool) { return unscalePID_i(PID_PARAM(Ki, tool)); }
    float getPIDValues_Kd(const atc_tool_t tool) { return unscalePID_d(PID_PARAM(Kd, tool)); }

    void setPIDValues(const_float_t p, const_float_t i, const_float_t d, atc_tool_t tool) {
      fanManager.temp_hotend[tool].pid.Kp = p;
      fanManager.temp_hotend[tool].pid.Ki = scalePID_i(i);
      fanManager.temp_hotend[tool].pid.Kd = scalePID_d(d);
      fanManager.updatePID();
    }

    void startPIDTune(const celsius_t temp, atc_tool_t tool) {
      fanManager.PID_autotune(temp, (heater_id_t)tool, 8, true);
    }
  #endif

  #if ENABLED(PIDTEMPBED)
    float getBedPIDValues_Kp() { return fanManager.temp_bed.pid.Kp; }
    float getBedPIDValues_Ki() { return unscalePID_i(fanManager.temp_bed.pid.Ki); }
    float getBedPIDValues_Kd() { return unscalePID_d(fanManager.temp_bed.pid.Kd); }

    void setBedPIDValues(const_float_t p, const_float_t i, const_float_t d) {
      fanManager.temp_bed.pid.Kp = p;
      fanManager.temp_bed.pid.Ki = scalePID_i(i);
      fanManager.temp_bed.pid.Kd = scalePID_d(d);
      fanManager.updatePID();
    }

    void startBedPIDTune(const celsius_t temp) {
      fanManager.PID_autotune(temp, H_BED, 4, true);
    }
  #endif

  void injectCommands_P(PGM_P const gcode) { queue.inject_P(gcode); }
  void injectCommands(char * const gcode)  { queue.inject(gcode); }

  bool commandsInQueue() { return (planner.movesplanned() || queue.has_commands_queued()); }

  bool isAxisPositionKnown(const axis_t axis) { return axis_is_trusted((AxisEnum)axis); }
  bool isAxisPositionKnown(const atc_tool_t) { return axis_is_trusted(E_AXIS); }
  bool isPositionKnown() { return all_axes_trusted(); }
  bool isMachineHomed() { return all_axes_homed(); }

  PGM_P getFirmwareName_str() {
    static PGMSTR(firmware_name, "mvCNC " SHORT_BUILD_VERSION);
    return firmware_name;
  }

  void setTargetTemp_celsius(const_float_t inval, const heater_t heater) {
    float value = inval;
    #ifdef TOUCH_UI_LCD_TEMP_SCALING
      value *= TOUCH_UI_LCD_TEMP_SCALING;
    #endif
    enableHeater(heater);
    switch (heater) {
      #if HAS_HEATED_CHAMBER
        case CHAMBER: fanManager.setTargetChamber(LROUND(constrain(value, 0, CHAMBER_MAX_TARGET))); break;
      #endif
      #if HAS_COOLER
        case COOLER: fanManager.setTargetCooler(LROUND(constrain(value, 0, COOLER_MAXTEMP))); break;
      #endif
      #if HAS_HEATED_BED
        case BED: fanManager.setTargetBed(LROUND(constrain(value, 0, BED_MAX_TARGET))); break;
      #endif
      default: {
        #if HAS_HOTEND
          const int16_t e = heater - H0;
          fanManager.setTargetHotend(LROUND(constrain(value, 0, fanManager.hotend_max_target(e))), e);
        #endif
      } break;
    }
  }

  void setTargetTemp_celsius(const_float_t inval, const atc_tool_t atc_tool) {
    float value = inval;
    #ifdef TOUCH_UI_LCD_TEMP_SCALING
      value *= TOUCH_UI_LCD_TEMP_SCALING;
    #endif
    #if HAS_HOTEND
      const int16_t e = atc_tool - E0;
      enableHeater(atc_tool);
      fanManager.setTargetHotend(LROUND(constrain(value, 0, fanManager.hotend_max_target(e))), e);
    #endif
  }

  void setTargetFan_percent(const_float_t value, const fan_t fan) {
    #if HAS_FAN
      if (fan < FAN_COUNT)
        fanManager.fanSpeedSet(fan - FAN0, map(constrain(value, 0, 100), 0, 100, 0, 255));
    #else
      UNUSED(value);
      UNUSED(fan);
    #endif
  }

  void setFeedrate_percent(const_float_t value) { feedrate_percentage = constrain(value, 10, 500); }

  void coolDown() { fanManager.cooldown(); }

  bool awaitingUserConfirm() {
    return TERN0(HAS_RESUME_CONTINUE, wait_for_user) || getHostKeepaliveIsPaused();
  }
  void setUserConfirmed() { TERN_(HAS_RESUME_CONTINUE, wait_for_user = false); }

  #if M600_PURGE_MORE_RESUMABLE
    void setPauseMenuResponse(PauseMenuResponse response) { pause_menu_response = response; }
    PauseMessage pauseModeStatus = PAUSE_MESSAGE_STATUS;
    PauseMode getPauseMode() { return pause_mode;}
  #endif

  void printFile(const char *filename) {
    TERN(SDSUPPORT, card.openAndPrintFile(filename), UNUSED(filename));
  }

  bool jobRunningFromMediaPaused() {
    return TERN0(SDSUPPORT, IS_SD_PAUSED());
  }

  bool jobRunningFromMedia() { return TERN0(SDSUPPORT, IS_SD_JOB_RUNNING() || IS_SD_PAUSED()); }

  bool jobRunning() { return commandsInQueue() || jobRunningFromMedia() || jobIsOngoing() || jobIsPaused(); }

  bool jobRunningPaused() {
    return jobRunning() && (jobRunningFromMediaPaused() || JobTimer.isPaused());
  }

  bool isMediaInserted() { return TERN0(SDSUPPORT, IS_SD_INSERTED()); }

  void pausePrint()  { ui.pause_print(); }
  void resumePrint() { ui.resume_print(); }
  void stopPrint()   { ui.abort_print(); }

  // Simplest approach is to make an SRAM copy
  void onUserConfirmRequired(FSTR_P const fstr) {
    char msg[strlen_P(FTOP(fstr)) + 1];
    strcpy_P(msg, FTOP(fstr));
    onUserConfirmRequired(msg);
  }

  void onStatusChanged(FSTR_P const fstr) {
    char msg[strlen_P(FTOP(fstr)) + 1];
    strcpy_P(msg, FTOP(fstr));
    onStatusChanged(msg);
  }

  FileList::FileList() { refresh(); }

  void FileList::refresh() { num_files = 0xFFFF; }

  bool FileList::seek(const uint16_t pos, const bool skip_range_check) {
    #if ENABLED(SDSUPPORT)
      if (!skip_range_check && (pos + 1) > count()) return false;
      card.getfilename_sorted(SD_ORDER(pos, count()));
      return card.filename[0] != '\0';
    #else
      UNUSED(pos);
      UNUSED(skip_range_check);
      return false;
    #endif
  }

  const char* FileList::filename() {
    return TERN(SDSUPPORT, card.longest_filename(), "");
  }

  const char* FileList::shortFilename() {
    return TERN(SDSUPPORT, card.filename, "");
  }

  const char* FileList::longFilename() {
    return TERN(SDSUPPORT, card.longFilename, "");
  }

  bool FileList::isDir() {
    return TERN0(SDSUPPORT, card.flag.filenameIsDir);
  }

  uint16_t FileList::count() {
    return TERN0(SDSUPPORT, (num_files = (num_files == 0xFFFF ? card.get_num_Files() : num_files)));
  }

  bool FileList::isAtRootDir() {
    return TERN1(SDSUPPORT, card.flag.workDirIsRoot);
  }

  void FileList::upDir() {
    #if ENABLED(SDSUPPORT)
      card.cdup();
      num_files = 0xFFFF;
    #endif
  }

  void FileList::changeDir(const char * const dirname) {
    #if ENABLED(SDSUPPORT)
      card.cd(dirname);
      num_files = 0xFFFF;
    #else
      UNUSED(dirname);
    #endif
  }

} // namespace ExtUI

// At the moment we hook into mvCNCUI methods, but this could be cleaned up in the future

void mvCNCUI::init_lcd() { ExtUI::onStartup(); }

void mvCNCUI::update() { ExtUI::onIdle(); }

void mvCNCUI::kill_screen(FSTR_P const error, FSTR_P const component) {
  using namespace ExtUI;
  if (!flags.cnc_killed) {
    flags.cnc_killed = true;
    onCNCKilled(error, component);
  }
}

#endif // EXTENSIBLE_UI
