/**
 * Modern Vintage CNC Firmware
*/

#include "../inc/mvCNCConfigPre.h"

#include "tool_change.h"

#include "probe.h"
#include "motion.h"
#include "planner.h"
#include "fan_control.h"

#include "../mvCNCCore.h"

//#define DEBUG_TOOL_CHANGE

#define DEBUG_OUT ENABLED(DEBUG_TOOL_CHANGE)
#include "../core/debug_out.h"

#if TOOL_CHANGE_SUPPORT
  toolchange_settings_t toolchange_settings;  // Initialized by settings.load()
#endif

#if ENABLED(TOOLCHANGE_MIGRATION_FEATURE)
  migration_settings_t migration = migration_defaults;
  bool enable_first_prime;
#endif

#if ENABLED(TOOLCHANGE_FS_INIT_BEFORE_SWAP)
  bool toolchange_atc_tool_ready[ATC_TOOLS];
#endif

#if EITHER(MAGNETIC_PARKING_EXTRUDER, TOOL_SENSOR) \
  || defined(EVENT_GCODE_TOOLCHANGE_T0) || defined(EVENT_GCODE_TOOLCHANGE_T1) || defined(EVENT_GCODE_AFTER_TOOLCHANGE) \
  || (ENABLED(PARKING_EXTRUDER) && PARKING_EXTRUDER_SOLENOIDS_DELAY > 0)
  #include "../gcode/gcode.h"
#endif

#if ENABLED(TOOL_SENSOR)
  #include "../lcd/mvcncui.h"
#endif

#if ENABLED(DUAL_X_CARRIAGE)
  #include "stepper.h"
#endif

#if ANY(SWITCHING_EXTRUDER, SWITCHING_NOZZLE, SWITCHING_TOOLHEAD)
  #include "servo.h"
#endif

#if ENABLED(EXT_SOLENOID) && DISABLED(PARKING_EXTRUDER)
  #include "../feature/solenoid.h"
#endif

#if ENABLED(MIXING_EXTRUDER)
  #include "../feature/mixing.h"
#endif

#if HAS_LEVELING
  #include "../feature/bedlevel/bedlevel.h"
#endif

#if HAS_FANMUX
  #include "../feature/fanmux.h"
#endif

#if HAS_PRUSA_MMU1
  #include "../feature/mmu/mmu.h"
#elif HAS_PRUSA_MMU2
  #include "../feature/mmu/mmu2.h"
#endif

#if HAS_MVCNCUI_MENU
  #include "../lcd/mvcncui.h"
#endif

#if ENABLED(ADVANCED_PAUSE_FEATURE)
  #include "../feature/pause.h"
#endif

#if ENABLED(TOOLCHANGE_FILAMENT_SWAP)
  #include "../gcode/gcode.h"
  #if TOOLCHANGE_FS_WIPE_RETRACT <= 0
    #undef TOOLCHANGE_FS_WIPE_RETRACT
    #define TOOLCHANGE_FS_WIPE_RETRACT 0
  #endif
#endif

#if DO_SWITCH_EXTRUDER

  #if ATC_TOOLS > 3
    #define _SERVO_NR(E) ((E) < 2 ? SWITCHING_EXTRUDER_SERVO_NR : SWITCHING_EXTRUDER_E23_SERVO_NR)
  #else
    #define _SERVO_NR(E) SWITCHING_EXTRUDER_SERVO_NR
  #endif

  void move_atc_tool_servo(const uint8_t e) {
    planner.synchronize();
    if ((ATC_TOOLS & 1) && e < ATC_TOOLS - 1) {
      MOVE_SERVO(_SERVO_NR(e), servo_angles[_SERVO_NR(e)][e & 1]);
      safe_delay(500);
    }
  }

#endif // DO_SWITCH_EXTRUDER

#if ENABLED(SWITCHING_NOZZLE)

  #if SWITCHING_NOZZLE_TWO_SERVOS

    inline void _move_tool_servo(const uint8_t e, const uint8_t angle_index) {
      constexpr int8_t  sns_index[2] = { SWITCHING_NOZZLE_SERVO_NR, SWITCHING_NOZZLE_E1_SERVO_NR };
      constexpr int16_t sns_angles[2] = SWITCHING_NOZZLE_SERVO_ANGLES;
      planner.synchronize();
      MOVE_SERVO(sns_index[e], sns_angles[angle_index]);
      safe_delay(500);
    }

    void lower_tool(const uint8_t e) { _move_tool_servo(e, 0); }
    void raise_tool(const uint8_t e) { _move_tool_servo(e, 1); }

  #else

    void move_tool_servo(const uint8_t angle_index) {
      planner.synchronize();
      MOVE_SERVO(SWITCHING_NOZZLE_SERVO_NR, servo_angles[SWITCHING_NOZZLE_SERVO_NR][angle_index]);
      safe_delay(500);
    }

  #endif

#endif // SWITCHING_NOZZLE

void _line_to_current(const AxisEnum fr_axis, const float fscale=1) {
  line_to_current_position(planner.settings.max_feedrate_mm_s[fr_axis] * fscale);
}
void slow_line_to_current(const AxisEnum fr_axis) { _line_to_current(fr_axis, 0.2f); }
void fast_line_to_current(const AxisEnum fr_axis) { _line_to_current(fr_axis, 0.5f); }

#if ENABLED(MAGNETIC_PARKING_EXTRUDER)

  float parkingposx[2],           // M951 R L
        parkinggrabdistance,      // M951 I
        parkingslowspeed,         // M951 J
        parkinghighspeed,         // M951 H
        parkingtraveldistance,    // M951 D
        compensationmultiplier;

  inline void magnetic_parking_atc_tool_tool_change(const uint8_t new_tool) {

    const float oldx = current_position.x,
                grabpos = mpe_settings.parking_xpos[new_tool] + (new_tool ? mpe_settings.grab_distance : -mpe_settings.grab_distance),
                offsetcompensation = TERN0(HAS_HOTEND_OFFSET, hotend_offset[active_tool].x * mpe_settings.compensation_factor);

    if (homing_needed_error(_BV(X_AXIS))) return;

    /**
     * Z Lift and Spindle Offset shift ar defined in caller method to work equal with any Multi Hotend realization
     *
     * Steps:
     *   1. Move high speed to park position of new ATC tool
     *   2. Move to couple position of new ATC tool (this also discouple the old ATC tool)
     *   3. Move to park position of new ATC tool
     *   4. Move high speed to approach park position of old ATC tool
     *   5. Move to park position of old ATC tool
     *   6. Move to starting position
     */

    // STEP 1

    current_position.x = mpe_settings.parking_xpos[new_tool] + offsetcompensation;

    DEBUG_ECHOPGM("(1) Move ATC tool ", new_tool);
    DEBUG_POS(" to new ATC tool ParkPos", current_position);

    planner.buffer_line(current_position, mpe_settings.fast_feedrate, new_tool);
    planner.synchronize();

    // STEP 2

    current_position.x = grabpos + offsetcompensation;

    DEBUG_ECHOPGM("(2) Couple ATC tool ", new_tool);
    DEBUG_POS(" to new ATC tool GrabPos", current_position);

    planner.buffer_line(current_position, mpe_settings.slow_feedrate, new_tool);
    planner.synchronize();

    // Delay before moving tool, to allow magnetic coupling
    gcode.dwell(150);

    // STEP 3

    current_position.x = mpe_settings.parking_xpos[new_tool] + offsetcompensation;

    DEBUG_ECHOPGM("(3) Move ATC tool ", new_tool);
    DEBUG_POS(" back to new ATC tool ParkPos", current_position);

    planner.buffer_line(current_position, mpe_settings.slow_feedrate, new_tool);
    planner.synchronize();

    // STEP 4

    current_position.x = mpe_settings.parking_xpos[active_tool] + (active_tool == 0 ? MPE_TRAVEL_DISTANCE : -MPE_TRAVEL_DISTANCE) + offsetcompensation;

    DEBUG_ECHOPGM("(4) Move ATC tool ", new_tool);
    DEBUG_POS(" close to old ATC tool ParkPos", current_position);

    planner.buffer_line(current_position, mpe_settings.fast_feedrate, new_tool);
    planner.synchronize();

    // STEP 5

    current_position.x = mpe_settings.parking_xpos[active_tool] + offsetcompensation;

    DEBUG_ECHOPGM("(5) Park ATC tool ", new_tool);
    DEBUG_POS(" at old ATC tool ParkPos", current_position);

    planner.buffer_line(current_position, mpe_settings.slow_feedrate, new_tool);
    planner.synchronize();

    // STEP 6

    current_position.x = oldx;

    DEBUG_ECHOPGM("(6) Move ATC tool ", new_tool);
    DEBUG_POS(" to starting position", current_position);

    planner.buffer_line(current_position, mpe_settings.fast_feedrate, new_tool);
    planner.synchronize();

    DEBUG_ECHOLNPGM("Autopark done.");
  }

#elif ENABLED(PARKING_EXTRUDER)

  void pe_solenoid_init() {
    LOOP_LE_N(n, 1) pe_solenoid_set_pin_state(n, !PARKING_EXTRUDER_SOLENOIDS_PINS_ACTIVE);
  }

  void pe_solenoid_set_pin_state(const uint8_t atc_tool_num, const uint8_t state) {
    switch (atc_tool_num) {
      case 1: OUT_WRITE(SOL1_PIN, state); break;
      default: OUT_WRITE(SOL0_PIN, state); break;
    }
    #if PARKING_EXTRUDER_SOLENOIDS_DELAY > 0
      gcode.dwell(PARKING_EXTRUDER_SOLENOIDS_DELAY);
    #endif
  }

  bool atc_tool_parked = true, do_solenoid_activation = true;

  // Modifies tool_change() behavior based on homing side
  bool parking_atc_tool_unpark_after_homing(const uint8_t final_tool, bool homed_towards_final_tool) {
    do_solenoid_activation = false; // Tell parking_atc_tool_tool_change to skip solenoid activation

    if (!atc_tool_parked) return false; // nothing to do

    if (homed_towards_final_tool) {
      pe_solenoid_magnet_off(1 - final_tool);
      DEBUG_ECHOLNPGM("Disengage magnet", 1 - final_tool);
      pe_solenoid_magnet_on(final_tool);
      DEBUG_ECHOLNPGM("Engage magnet", final_tool);
      parking_atc_tool_set_parked(false);
      return false;
    }

    return true;
  }

  inline void parking_atc_tool_tool_change(const uint8_t new_tool, bool no_move) {
    if (!no_move) {

      constexpr float parkingposx[] = PARKING_EXTRUDER_PARKING_X;

      #if HAS_HOTEND_OFFSET
        const float x_offset = hotend_offset[active_tool].x;
      #else
        constexpr float x_offset = 0;
      #endif

      const float midpos = (parkingposx[0] + parkingposx[1]) * 0.5f + x_offset,
                  grabpos = parkingposx[new_tool] + (new_tool ? PARKING_EXTRUDER_GRAB_DISTANCE : -(PARKING_EXTRUDER_GRAB_DISTANCE)) + x_offset;

      /**
       * 1. Move to park position of old ATC tool
       * 2. Disengage magnetic field, wait for delay
       * 3. Move near new ATC tool
       * 4. Engage magnetic field for new ATC tool
       * 5. Move to parking incl. offset of new ATC tool
       * 6. Lower Z-Axis
       */

      // STEP 1

      DEBUG_POS("Start PE Tool-Change", current_position);

      // Don't park the active_tool unless unparked
      if (!atc_tool_parked) {
        current_position.x = parkingposx[active_tool] + x_offset;

        DEBUG_ECHOLNPGM("(1) Park ATC tool ", active_tool);
        DEBUG_POS("Moving ParkPos", current_position);

        fast_line_to_current(X_AXIS);

        // STEP 2

        planner.synchronize();
        DEBUG_ECHOLNPGM("(2) Disengage magnet");
        pe_solenoid_magnet_off(active_tool);

        // STEP 3

        current_position.x += active_tool ? -10 : 10; // move 10mm away from parked ATC tool

        DEBUG_ECHOLNPGM("(3) Move near new ATC tool");
        DEBUG_POS("Move away from parked ATC tool", current_position);

        fast_line_to_current(X_AXIS);
      }

      // STEP 4

      planner.synchronize();
      DEBUG_ECHOLNPGM("(4) Engage magnetic field");

      // Just save power for inverted magnets
      TERN_(PARKING_EXTRUDER_SOLENOIDS_INVERT, pe_solenoid_magnet_on(active_tool));
      pe_solenoid_magnet_on(new_tool);

      // STEP 5

      current_position.x = grabpos + (new_tool ? -10 : 10);
      fast_line_to_current(X_AXIS);

      current_position.x = grabpos;

      DEBUG_SYNCHRONIZE();
      DEBUG_POS("(5) Unpark ATC tool", current_position);

      slow_line_to_current(X_AXIS);

      // STEP 6

      current_position.x = DIFF_TERN(HAS_HOTEND_OFFSET, midpos, hotend_offset[new_tool].x);

      DEBUG_SYNCHRONIZE();
      DEBUG_POS("(6) Move midway between hotends", current_position);

      fast_line_to_current(X_AXIS);
      planner.synchronize(); // Always sync the final move

      DEBUG_POS("PE Tool-Change done.", current_position);
      parking_atc_tool_set_parked(false);
    }
    else if (do_solenoid_activation) {
      // Deactivate current ATC tool solenoid
      pe_solenoid_set_pin_state(active_tool, !PARKING_EXTRUDER_SOLENOIDS_PINS_ACTIVE);
      // Engage new ATC tool magnetic field
      pe_solenoid_set_pin_state(new_tool, PARKING_EXTRUDER_SOLENOIDS_PINS_ACTIVE);
    }

    do_solenoid_activation = true; // Activate solenoid for subsequent tool_change()
  }

#endif // PARKING_EXTRUDER

#if ENABLED(SWITCHING_TOOLHEAD)

  // Return a bitmask of tool sensor states
  inline uint8_t poll_tool_sensor_pins() {
    return (0
      #if ENABLED(TOOL_SENSOR)
        #if PIN_EXISTS(TOOL_SENSOR1)
          | (READ(TOOL_SENSOR1_PIN) << 0)
        #endif
        #if PIN_EXISTS(TOOL_SENSOR2)
          | (READ(TOOL_SENSOR2_PIN) << 1)
        #endif
        #if PIN_EXISTS(TOOL_SENSOR3)
          | (READ(TOOL_SENSOR3_PIN) << 2)
        #endif
        #if PIN_EXISTS(TOOL_SENSOR4)
          | (READ(TOOL_SENSOR4_PIN) << 3)
        #endif
        #if PIN_EXISTS(TOOL_SENSOR5)
          | (READ(TOOL_SENSOR5_PIN) << 4)
        #endif
        #if PIN_EXISTS(TOOL_SENSOR6)
          | (READ(TOOL_SENSOR6_PIN) << 5)
        #endif
        #if PIN_EXISTS(TOOL_SENSOR7)
          | (READ(TOOL_SENSOR7_PIN) << 6)
        #endif
        #if PIN_EXISTS(TOOL_SENSOR8)
          | (READ(TOOL_SENSOR8_PIN) << 7)
        #endif
      #endif
    );
  }

  #if ENABLED(TOOL_SENSOR)

    bool tool_sensor_disabled; // = false

    uint8_t check_tool_sensor_stats(const uint8_t tool_index, const bool kill_on_error/*=false*/, const bool disable/*=false*/) {
      static uint8_t sensor_tries; // = 0
      for (;;) {
        if (poll_tool_sensor_pins() == _BV(tool_index)) {
          sensor_tries = 0;
          return tool_index;
        }
        else if (kill_on_error && (!tool_sensor_disabled || disable)) {
          sensor_tries++;
          if (sensor_tries > 10) kill(F("Tool Sensor error"));
          safe_delay(5);
        }
        else {
          sensor_tries++;
          if (sensor_tries > 10) return -1;
          safe_delay(5);
        }
      }
    }

  #endif

  inline void switching_toolhead_lock(const bool locked) {
    #ifdef SWITCHING_TOOLHEAD_SERVO_ANGLES
      const uint16_t swt_angles[2] = SWITCHING_TOOLHEAD_SERVO_ANGLES;
      MOVE_SERVO(SWITCHING_TOOLHEAD_SERVO_NR, swt_angles[locked ? 0 : 1]);
    #elif PIN_EXISTS(SWT_SOLENOID)
      OUT_WRITE(SWT_SOLENOID_PIN, locked);
      gcode.dwell(10);
    #else
      #error "No toolhead locking mechanism configured."
    #endif
  }

  #include <bitset>

  void swt_init() {
    switching_toolhead_lock(true);

    #if ENABLED(TOOL_SENSOR)
      // Init tool sensors
      #if PIN_EXISTS(TOOL_SENSOR1)
        SET_INPUT_PULLUP(TOOL_SENSOR1_PIN);
      #endif
      #if PIN_EXISTS(TOOL_SENSOR2)
        SET_INPUT_PULLUP(TOOL_SENSOR2_PIN);
      #endif
      #if PIN_EXISTS(TOOL_SENSOR3)
        SET_INPUT_PULLUP(TOOL_SENSOR3_PIN);
      #endif
      #if PIN_EXISTS(TOOL_SENSOR4)
        SET_INPUT_PULLUP(TOOL_SENSOR4_PIN);
      #endif
      #if PIN_EXISTS(TOOL_SENSOR5)
        SET_INPUT_PULLUP(TOOL_SENSOR5_PIN);
      #endif
      #if PIN_EXISTS(TOOL_SENSOR6)
        SET_INPUT_PULLUP(TOOL_SENSOR6_PIN);
      #endif
      #if PIN_EXISTS(TOOL_SENSOR7)
        SET_INPUT_PULLUP(TOOL_SENSOR7_PIN);
      #endif
      #if PIN_EXISTS(TOOL_SENSOR8)
        SET_INPUT_PULLUP(TOOL_SENSOR8_PIN);
      #endif

      if (check_tool_sensor_stats(0)) {
        LCD_MESSAGE_F("TC error");
        switching_toolhead_lock(false);
        while (check_tool_sensor_stats(0)) { /* nada */ }
        switching_toolhead_lock(true);
      }
      LCD_MESSAGE_F("TC Success");
    #endif
  }

  inline void switching_toolhead_tool_change(const uint8_t new_tool, bool no_move/*=false*/) {
    if (no_move) return;

    constexpr float toolheadposx[] = SWITCHING_TOOLHEAD_X_POS;
    const float placexpos = toolheadposx[active_tool],
                grabxpos = toolheadposx[new_tool];

    (void)check_tool_sensor_stats(active_tool, true);

    /**
     * 1. Move to switch position of current toolhead
     * 2. Unlock tool and drop it in the dock
     * 3. Move to the new toolhead
     * 4. Grab and lock the new toolhead
     */

    // 1. Move to switch position of current toolhead

    DEBUG_POS("Start ST Tool-Change", current_position);

    current_position.x = placexpos;

    DEBUG_ECHOLNPGM("(1) Place old tool ", active_tool);
    DEBUG_POS("Move X SwitchPos", current_position);

    fast_line_to_current(X_AXIS);

    current_position.y = SWITCHING_TOOLHEAD_Y_POS - (SWITCHING_TOOLHEAD_Y_SECURITY);

    DEBUG_SYNCHRONIZE();
    DEBUG_POS("Move Y SwitchPos + Security", current_position);

    slow_line_to_current(Y_AXIS);

    // 2. Unlock tool and drop it in the dock
    TERN_(TOOL_SENSOR, tool_sensor_disabled = true);

    planner.synchronize();
    DEBUG_ECHOLNPGM("(2) Unlock and Place Toolhead");
    switching_toolhead_lock(false);
    safe_delay(500);

    current_position.y = SWITCHING_TOOLHEAD_Y_POS;
    DEBUG_POS("Move Y SwitchPos", current_position);
    slow_line_to_current(Y_AXIS);

    // Wait for move to complete, then another 0.2s
    planner.synchronize();
    safe_delay(200);

    current_position.y -= SWITCHING_TOOLHEAD_Y_CLEAR;
    DEBUG_POS("Move back Y clear", current_position);
    slow_line_to_current(Y_AXIS); // move away from docked toolhead

    (void)check_tool_sensor_stats(active_tool);

    // 3. Move to the new toolhead

    current_position.x = grabxpos;

    DEBUG_SYNCHRONIZE();
    DEBUG_ECHOLNPGM("(3) Move to new toolhead position");
    DEBUG_POS("Move to new toolhead X", current_position);

    fast_line_to_current(X_AXIS);

    current_position.y = SWITCHING_TOOLHEAD_Y_POS - (SWITCHING_TOOLHEAD_Y_SECURITY);

    DEBUG_SYNCHRONIZE();
    DEBUG_POS("Move Y SwitchPos + Security", current_position);

    slow_line_to_current(Y_AXIS);

    // 4. Grab and lock the new toolhead

    current_position.y = SWITCHING_TOOLHEAD_Y_POS;

    DEBUG_SYNCHRONIZE();
    DEBUG_ECHOLNPGM("(4) Grab and lock new toolhead");
    DEBUG_POS("Move Y SwitchPos", current_position);

    slow_line_to_current(Y_AXIS);

    // Wait for move to finish, pause 0.2s, move servo, pause 0.5s
    planner.synchronize();
    safe_delay(200);

    (void)check_tool_sensor_stats(new_tool, true, true);

    switching_toolhead_lock(true);
    safe_delay(500);

    current_position.y -= SWITCHING_TOOLHEAD_Y_CLEAR;
    DEBUG_POS("Move back Y clear", current_position);
    slow_line_to_current(Y_AXIS); // Move away from docked toolhead
    planner.synchronize();        // Always sync the final move

    (void)check_tool_sensor_stats(new_tool, true, true);

    DEBUG_POS("ST Tool-Change done.", current_position);
  }

#elif ENABLED(MAGNETIC_SWITCHING_TOOLHEAD)

  inline void magnetic_switching_toolhead_tool_change(const uint8_t new_tool, bool no_move/*=false*/) {
    if (no_move) return;

    constexpr float toolheadposx[] = SWITCHING_TOOLHEAD_X_POS,
                    toolheadclearx[] = SWITCHING_TOOLHEAD_X_SECURITY;

    const float placexpos = toolheadposx[active_tool],
                placexclear = toolheadclearx[active_tool],
                grabxpos = toolheadposx[new_tool],
                grabxclear = toolheadclearx[new_tool];

    /**
     * 1. Move to switch position of current toolhead
     * 2. Release and place toolhead in the dock
     * 3. Move to the new toolhead
     * 4. Grab the new toolhead and move to security position
     */

    DEBUG_POS("Start MST Tool-Change", current_position);

    // 1. Move to switch position current toolhead

    current_position.y = SWITCHING_TOOLHEAD_Y_POS + SWITCHING_TOOLHEAD_Y_CLEAR;

    SERIAL_ECHOLNPGM("(1) Place old tool ", active_tool);
    DEBUG_POS("Move Y SwitchPos + Security", current_position);

    fast_line_to_current(Y_AXIS);

    current_position.x = placexclear;

    DEBUG_SYNCHRONIZE();
    DEBUG_POS("Move X SwitchPos + Security", current_position);

    fast_line_to_current(X_AXIS);

    current_position.y = SWITCHING_TOOLHEAD_Y_POS;

    DEBUG_SYNCHRONIZE();
    DEBUG_POS("Move Y SwitchPos", current_position);

    fast_line_to_current(Y_AXIS);

    current_position.x = placexpos;

    DEBUG_SYNCHRONIZE();
    DEBUG_POS("Move X SwitchPos", current_position);

    line_to_current_position(planner.settings.max_feedrate_mm_s[X_AXIS] * 0.25f);

    // 2. Release and place toolhead in the dock

    DEBUG_SYNCHRONIZE();
    DEBUG_ECHOLNPGM("(2) Release and Place Toolhead");

    current_position.y = SWITCHING_TOOLHEAD_Y_POS + SWITCHING_TOOLHEAD_Y_RELEASE;
    DEBUG_POS("Move Y SwitchPos + Release", current_position);
    line_to_current_position(planner.settings.max_feedrate_mm_s[Y_AXIS] * 0.1f);

    current_position.y = SWITCHING_TOOLHEAD_Y_POS + SWITCHING_TOOLHEAD_Y_SECURITY;

    DEBUG_SYNCHRONIZE();
    DEBUG_POS("Move Y SwitchPos + Security", current_position);

    line_to_current_position(planner.settings.max_feedrate_mm_s[Y_AXIS]);

    // 3. Move to new toolhead position

    DEBUG_SYNCHRONIZE();
    DEBUG_ECHOLNPGM("(3) Move to new toolhead position");

    current_position.x = grabxpos;
    DEBUG_POS("Move to new toolhead X", current_position);
    fast_line_to_current(X_AXIS);

    // 4. Grab the new toolhead and move to security position

    DEBUG_SYNCHRONIZE();
    DEBUG_ECHOLNPGM("(4) Grab new toolhead, move to security position");

    current_position.y = SWITCHING_TOOLHEAD_Y_POS + SWITCHING_TOOLHEAD_Y_RELEASE;
    DEBUG_POS("Move Y SwitchPos + Release", current_position);
    line_to_current_position(planner.settings.max_feedrate_mm_s[Y_AXIS]);

    current_position.y = SWITCHING_TOOLHEAD_Y_POS;

    DEBUG_SYNCHRONIZE();
    DEBUG_POS("Move Y SwitchPos", current_position);

    _line_to_current(Y_AXIS, 0.2f);

    #if ENABLED(PRIME_BEFORE_REMOVE) && (SWITCHING_TOOLHEAD_PRIME_MM || SWITCHING_TOOLHEAD_RETRACT_MM)
      #if SWITCHING_TOOLHEAD_PRIME_MM
        current_position.e += SWITCHING_TOOLHEAD_PRIME_MM;
        planner.buffer_line(current_position, MMM_TO_MMS(SWITCHING_TOOLHEAD_PRIME_FEEDRATE), new_tool);
      #endif
      #if SWITCHING_TOOLHEAD_RETRACT_MM
        current_position.e -= SWITCHING_TOOLHEAD_RETRACT_MM;
        planner.buffer_line(current_position, MMM_TO_MMS(SWITCHING_TOOLHEAD_RETRACT_FEEDRATE), new_tool);
      #endif
    #else
      planner.synchronize();
      safe_delay(100); // Give switch time to settle
    #endif

    current_position.x = grabxclear;
    DEBUG_POS("Move to new toolhead X + Security", current_position);
    _line_to_current(X_AXIS, 0.1f);
    planner.synchronize();
    safe_delay(100); // Give switch time to settle

    current_position.y += SWITCHING_TOOLHEAD_Y_CLEAR;
    DEBUG_POS("Move back Y clear", current_position);
    fast_line_to_current(Y_AXIS); // move away from docked toolhead
    planner.synchronize(); // Always sync last tool-change move

    DEBUG_POS("MST Tool-Change done.", current_position);
  }

#elif ENABLED(ELECTROMAGNETIC_SWITCHING_TOOLHEAD)

  inline void est_activate_solenoid()   { OUT_WRITE(SOL0_PIN, HIGH); }
  inline void est_deactivate_solenoid() { OUT_WRITE(SOL0_PIN, LOW); }
  void est_init() { est_activate_solenoid(); }

  inline void em_switching_toolhead_tool_change(const uint8_t new_tool, bool no_move) {
    if (no_move) return;

    constexpr float toolheadposx[] = SWITCHING_TOOLHEAD_X_POS;
    const float placexpos = toolheadposx[active_tool],
                grabxpos = toolheadposx[new_tool];
    const xyz_pos_t &hoffs = hotend_offset[active_tool];

    /**
     * 1. Raise Z-Axis to give enough clearance
     * 2. Move to position near active ATC tool parking
     * 3. Move gently to park position of active ATC tool
     * 4. Disengage magnetic field, wait for delay
     * 5. Leave ATC tool and move to position near new ATC tool parking
     * 6. Move gently to park position of new ATC tool
     * 7. Engage magnetic field for new ATC tool parking
     * 8. Unpark ATC tool
     * 9. Apply Z hotend offset to current position
     */

    DEBUG_POS("Start EMST Tool-Change", current_position);

    // 1. Raise Z-Axis to give enough clearance

    current_position.z += SWITCHING_TOOLHEAD_Z_HOP;
    DEBUG_POS("(1) Raise Z-Axis ", current_position);
    fast_line_to_current(Z_AXIS);

    // 2. Move to position near active ATC tool parking

    DEBUG_SYNCHRONIZE();
    DEBUG_ECHOLNPGM("(2) Move near active ATC tool parking", active_tool);
    DEBUG_POS("Moving ParkPos", current_position);

    current_position.set(hoffs.x + placexpos,
                         hoffs.y + SWITCHING_TOOLHEAD_Y_POS + SWITCHING_TOOLHEAD_Y_CLEAR);
    fast_line_to_current(X_AXIS);

    // 3. Move gently to park position of active ATC tool

    DEBUG_SYNCHRONIZE();
    SERIAL_ECHOLNPGM("(3) Move gently to park position of active ATC tool", active_tool);
    DEBUG_POS("Moving ParkPos", current_position);

    current_position.y -= SWITCHING_TOOLHEAD_Y_CLEAR;
    slow_line_to_current(Y_AXIS);

    // 4. Disengage magnetic field, wait for delay

    planner.synchronize();
    DEBUG_ECHOLNPGM("(4) Disengage magnet");
    est_deactivate_solenoid();

    // 5. Leave ATC tool and move to position near new ATC tool parking

    DEBUG_ECHOLNPGM("(5) Move near new ATC tool parking");
    DEBUG_POS("Moving ParkPos", current_position);

    current_position.y += SWITCHING_TOOLHEAD_Y_CLEAR;
    slow_line_to_current(Y_AXIS);
    current_position.set(hoffs.x + grabxpos,
                         hoffs.y + SWITCHING_TOOLHEAD_Y_POS + SWITCHING_TOOLHEAD_Y_CLEAR);
    fast_line_to_current(X_AXIS);

    // 6. Move gently to park position of new ATC tool

    current_position.y -= SWITCHING_TOOLHEAD_Y_CLEAR;
    if (DEBUGGING(LEVELING)) {
      planner.synchronize();
      DEBUG_ECHOLNPGM("(6) Move near new ATC tool");
    }
    slow_line_to_current(Y_AXIS);

    // 7. Engage magnetic field for new ATC tool parking

    DEBUG_SYNCHRONIZE();
    DEBUG_ECHOLNPGM("(7) Engage magnetic field");
    est_activate_solenoid();

    // 8. Unpark ATC tool

    current_position.y += SWITCHING_TOOLHEAD_Y_CLEAR;
    DEBUG_ECHOLNPGM("(8) Unpark ATC tool");
    slow_line_to_current(X_AXIS);
    planner.synchronize(); // Always sync the final move

    // 9. Apply Z hotend offset to current position

    DEBUG_POS("(9) Applying Z-offset", current_position);
    current_position.z += hoffs.z - hotend_offset[new_tool].z;

    DEBUG_POS("EMST Tool-Change done.", current_position);
  }

#endif // ELECTROMAGNETIC_SWITCHING_TOOLHEAD

#if ENABLED(DUAL_X_CARRIAGE)

  /**
   * @brief Dual X Tool Change
   * @details Change tools, with extra behavior based on current mode
   *
   * @param new_tool Tool index to activate
   * @param no_move Flag indicating no moves should take place
   */
  inline void dualx_tool_change(const uint8_t new_tool, bool &no_move) {

    DEBUG_ECHOPGM("Dual X Carriage Mode ");
    switch (dual_x_carriage_mode) {
      case DXC_FULL_CONTROL_MODE: DEBUG_ECHOLNPGM("FULL_CONTROL"); break;
      case DXC_AUTO_PARK_MODE:    DEBUG_ECHOLNPGM("AUTO_PARK");    break;
      case DXC_DUPLICATION_MODE:  DEBUG_ECHOLNPGM("DUPLICATION");  break;
      case DXC_MIRRORED_MODE:     DEBUG_ECHOLNPGM("MIRRORED");     break;
    }

    // Get the home position of the currently-active tool
    const float xhome = x_home_pos(active_tool);

    if (dual_x_carriage_mode == DXC_AUTO_PARK_MODE                  // If Auto-Park mode is enabled
        && IsRunning() && !no_move                                  // ...and movement is permitted
        && (delayed_move_time || current_position.x != xhome)       // ...and delayed_move_time is set OR not "already parked"...
    ) {
      DEBUG_ECHOLNPGM("MoveX to ", xhome);
      current_position.x = xhome;
      line_to_current_position(planner.settings.max_feedrate_mm_s[X_AXIS]);   // Park the current head
      planner.synchronize();
    }

    // Activate the new ATC tool ahead of calling set_axis_is_at_home!
    active_tool = new_tool;

    // This function resets the max/min values - the current position may be overwritten below.
    set_axis_is_at_home(X_AXIS);

    DEBUG_POS("New Extruder", current_position);

    switch (dual_x_carriage_mode) {
      case DXC_FULL_CONTROL_MODE:
        // New current position is the position of the activated ATC tool
        current_position.x = inactive_tool_x;
        // Save the inactive ATC tool's position (from the old current_position)
        inactive_tool_x = destination.x;
        DEBUG_ECHOLNPGM("DXC Full Control curr.x=", current_position.x, " dest.x=", destination.x);
        break;
      case DXC_AUTO_PARK_MODE:
        idex_set_parked();
        break;
      default:
        break;
    }

    // Ensure X axis DIR pertains to the correct carriage
    stepper.set_directions();

    DEBUG_ECHOLNPGM("Active ATC tool parked: ", active_tool_parked ? "yes" : "no");
    DEBUG_POS("New ATC tool (parked)", current_position);
  }

#endif // DUAL_X_CARRIAGE

/**
 * Prime active tool using TOOLCHANGE_FILAMENT_SWAP settings
 */
#if ENABLED(TOOLCHANGE_FILAMENT_SWAP)

  void tool_change_prime() {
    if (toolchange_settings.extra_prime > 0
      && TERN(PREVENT_COLD_EXTRUSION, !fanManager.targetTooColdToExtrude(active_tool), 1)
    ) {
      destination = current_position; // Remember the old position

      const bool ok = TERN1(TOOLCHANGE_PARK, all_axes_homed() && toolchange_settings.enable_park);

      #if HAS_FAN && TOOLCHANGE_FS_FAN >= 0
        // Store and stop fan. Restored on any exit.
        REMEMBER(fan, fanManager.fan_speed[TOOLCHANGE_FS_FAN], 0);
      #endif

      // Z raise
      if (ok) {
        // Do a small lift to avoid the workpiece in the move back (below)
        current_position.z += toolchange_settings.z_raise;
        TERN_(HAS_SOFTWARE_ENDSTOPS, NOMORE(current_position.z, soft_endstop.max.z));
        fast_line_to_current(Z_AXIS);
        planner.synchronize();
      }

      // Park
      #if ENABLED(TOOLCHANGE_PARK)
        if (ok) {
          IF_DISABLED(TOOLCHANGE_PARK_Y_ONLY, current_position.x = toolchange_settings.change_point.x);
          IF_DISABLED(TOOLCHANGE_PARK_X_ONLY, current_position.y = toolchange_settings.change_point.y);
          planner.buffer_line(current_position, MMM_TO_MMS(TOOLCHANGE_PARK_XY_FEEDRATE), active_tool);
          planner.synchronize();
        }
      #endif

      // Prime (All distances are added and slowed down to ensure secure priming in all circumstances)
      unscaled_e_move(toolchange_settings.swap_length + toolchange_settings.extra_prime, MMM_TO_MMS(toolchange_settings.prime_speed));

      // Cutting retraction
      #if TOOLCHANGE_FS_WIPE_RETRACT
        unscaled_e_move(-(TOOLCHANGE_FS_WIPE_RETRACT), MMM_TO_MMS(toolchange_settings.retract_speed));
      #endif

      // Cool down with fan
      #if HAS_FAN && TOOLCHANGE_FS_FAN >= 0
        fanManager.fan_speed[TOOLCHANGE_FS_FAN] = toolchange_settings.fan_speed;
        gcode.dwell(SEC_TO_MS(toolchange_settings.fan_time));
        fanManager.fan_speed[TOOLCHANGE_FS_FAN] = 0;
      #endif

      // Move back
      #if ENABLED(TOOLCHANGE_PARK)
        if (ok) {
          #if ENABLED(TOOLCHANGE_NO_RETURN)
            const float temp = destination.z;
            destination = current_position;
            destination.z = temp;
          #endif
          prepare_internal_move_to_destination(TERN(TOOLCHANGE_NO_RETURN, planner.settings.max_feedrate_mm_s[Z_AXIS], MMM_TO_MMS(TOOLCHANGE_PARK_XY_FEEDRATE)));
        }
      #endif

      // Cutting recover
      unscaled_e_move(toolchange_settings.extra_resume + TOOLCHANGE_FS_WIPE_RETRACT, MMM_TO_MMS(toolchange_settings.unretract_speed));

      // Resume at the old E position
      current_position.e = destination.e;
      sync_plan_position_e();
    }
  }

#endif // TOOLCHANGE_FILAMENT_SWAP

/**
 * Perform a tool-change, which may result in moving the
 * previous tool out of the way and the new tool into place.
 */
void tool_change(const uint8_t new_tool, bool no_move/*=false*/) {

  if (TERN0(MAGNETIC_SWITCHING_TOOLHEAD, new_tool == active_tool))
    return;

  #if ENABLED(MIXING_EXTRUDER)

    UNUSED(no_move);

    if (new_tool >= MIXING_VIRTUAL_TOOLS)
      return invalid_atc_tool_error(new_tool);

    #if MIXING_VIRTUAL_TOOLS > 1
      // T0-Tnnn: Switch virtual tool by changing the index to the mix
      mixer.T(new_tool);
    #endif

  #elif HAS_PRUSA_MMU2

    UNUSED(no_move);

    mmu2.tool_change(new_tool);

  #elif ATC_TOOLS == 0

    // Nothing to do
    UNUSED(new_tool); UNUSED(no_move);

  #elif ATC_TOOLS < 2

    UNUSED(no_move);

    if (new_tool) invalid_atc_tool_error(new_tool);
    return;

  #elif TOOL_CHANGE_SUPPORT

    planner.synchronize();

    #if ENABLED(DUAL_X_CARRIAGE)  // Only T0 allowed if the CNC is in DXC_DUPLICATION_MODE or DXC_MIRRORED_MODE
      if (new_tool != 0 && idex_is_duplicating())
         return invalid_atc_tool_error(new_tool);
    #endif

    if (new_tool >= ATC_TOOLS)
      return invalid_atc_tool_error(new_tool);

    if (!no_move && homing_needed()) {
      no_move = true;
      DEBUG_ECHOLNPGM("No move (not homed)");
    }

    TERN_(HAS_MVCNCUI_MENU, if (!no_move) ui.update());

    #if ENABLED(DUAL_X_CARRIAGE)
      const bool idex_full_control = dual_x_carriage_mode == DXC_FULL_CONTROL_MODE;
    #else
      constexpr bool idex_full_control = false;
    #endif

    const uint8_t old_tool = active_tool;
    const bool can_move_away = !no_move && !idex_full_control;

    #if HAS_LEVELING
      // Set current position to the physical position
      TEMPORARY_BED_LEVELING_STATE(false);
    #endif

    // First tool priming. To prime again, reboot the machine.
    #if ENABLED(TOOLCHANGE_FS_PRIME_FIRST_USED)
      static bool first_tool_is_primed = false;
      if (new_tool == old_tool && !first_tool_is_primed && enable_first_prime) {
        tool_change_prime();
        first_tool_is_primed = true;
        TERN_(TOOLCHANGE_FS_INIT_BEFORE_SWAP, toolchange_atc_tool_ready[old_tool] = true); // Primed and initialized
      }
    #endif

    if (new_tool != old_tool || TERN0(PARKING_EXTRUDER, atc_tool_parked)) { // PARKING_EXTRUDER may need to attach old_tool when homing
      destination = current_position;

      #if BOTH(TOOLCHANGE_FILAMENT_SWAP, HAS_FAN) && TOOLCHANGE_FS_FAN >= 0
        // Store and stop fan. Restored on any exit.
        REMEMBER(fan, fanManager.fan_speed[TOOLCHANGE_FS_FAN], 0);
      #endif

      // Z raise before retraction
      #if ENABLED(TOOLCHANGE_ZRAISE_BEFORE_RETRACT) && DISABLED(SWITCHING_NOZZLE)
        if (can_move_away && TERN1(TOOLCHANGE_PARK, toolchange_settings.enable_park)) {
          // Do a small lift to avoid the workpiece in the move back (below)
          current_position.z += toolchange_settings.z_raise;
          TERN_(HAS_SOFTWARE_ENDSTOPS, NOMORE(current_position.z, soft_endstop.max.z));
          fast_line_to_current(Z_AXIS);
          planner.synchronize();
        }
      #endif

      // Unload / Retract
      #if ENABLED(TOOLCHANGE_FILAMENT_SWAP)
        const bool should_swap = can_move_away && toolchange_settings.swap_length,
                   too_cold = TERN0(PREVENT_COLD_EXTRUSION,
                     !DEBUGGING(DRYRUN) && (fanManager.targetTooColdToExtrude(old_tool) || fanManager.targetTooColdToExtrude(new_tool))
                   );
        if (should_swap) {
          if (too_cold) {
            SERIAL_ECHO_MSG(STR_ERR_HOTEND_TOO_COLD);
            if (ENABLED(SINGLENOZZLE)) { active_tool = new_tool; return; }
          }
          else {
            // For first new tool, change without unloading the old. 'Just prime/init the new'
            if (TERN1(TOOLCHANGE_FS_PRIME_FIRST_USED, first_tool_is_primed))
              unscaled_e_move(-toolchange_settings.swap_length, MMM_TO_MMS(toolchange_settings.retract_speed));
            TERN_(TOOLCHANGE_FS_PRIME_FIRST_USED, first_tool_is_primed = true); // The first new tool will be primed by toolchanging
          }
        }
      #endif

      TERN_(SWITCHING_NOZZLE_TWO_SERVOS, raise_tool(old_tool));

      REMEMBER(fr, feedrate_mm_s, XY_PROBE_FEEDRATE_MM_S);

      #if HAS_SOFTWARE_ENDSTOPS
        #if HAS_HOTEND_OFFSET
          #define _EXT_ARGS , old_tool, new_tool
        #else
          #define _EXT_ARGS
        #endif
        update_software_endstops(X_AXIS _EXT_ARGS);
        #if DISABLED(DUAL_X_CARRIAGE)
          update_software_endstops(Y_AXIS _EXT_ARGS);
          update_software_endstops(Z_AXIS _EXT_ARGS);
        #endif
      #endif

      #if DISABLED(TOOLCHANGE_ZRAISE_BEFORE_RETRACT) && DISABLED(SWITCHING_NOZZLE)
        if (can_move_away && TERN1(TOOLCHANGE_PARK, toolchange_settings.enable_park)) {
          // Do a small lift to avoid the workpiece in the move back (below)
          current_position.z += toolchange_settings.z_raise;
          TERN_(HAS_SOFTWARE_ENDSTOPS, NOMORE(current_position.z, soft_endstop.max.z));
          fast_line_to_current(Z_AXIS);
        }
      #endif

      // Toolchange park
      #if ENABLED(TOOLCHANGE_PARK) && DISABLED(SWITCHING_NOZZLE)
        if (can_move_away && toolchange_settings.enable_park) {
          IF_DISABLED(TOOLCHANGE_PARK_Y_ONLY, current_position.x = toolchange_settings.change_point.x);
          IF_DISABLED(TOOLCHANGE_PARK_X_ONLY, current_position.y = toolchange_settings.change_point.y);
          planner.buffer_line(current_position, MMM_TO_MMS(TOOLCHANGE_PARK_XY_FEEDRATE), old_tool);
          planner.synchronize();
        }
      #endif

      #if HAS_HOTEND_OFFSET
        xyz_pos_t diff = hotend_offset[new_tool] - hotend_offset[old_tool];
        TERN_(DUAL_X_CARRIAGE, diff.x = 0);
      #else
        constexpr xyz_pos_t diff{0};
      #endif

      #if ENABLED(DUAL_X_CARRIAGE)
        dualx_tool_change(new_tool, no_move);
      #elif ENABLED(PARKING_EXTRUDER)                                   // Dual Parking ATC tool
        parking_atc_tool_tool_change(new_tool, no_move);
      #elif ENABLED(MAGNETIC_PARKING_EXTRUDER)                          // Magnetic Parking ATC tool
        magnetic_parking_atc_tool_tool_change(new_tool);
      #elif ENABLED(SWITCHING_TOOLHEAD)                                 // Switching Toolhead
        switching_toolhead_tool_change(new_tool, no_move);
      #elif ENABLED(MAGNETIC_SWITCHING_TOOLHEAD)                        // Magnetic Switching Toolhead
        magnetic_switching_toolhead_tool_change(new_tool, no_move);
      #elif ENABLED(ELECTROMAGNETIC_SWITCHING_TOOLHEAD)                 // Magnetic Switching ToolChanger
        em_switching_toolhead_tool_change(new_tool, no_move);
      #elif ENABLED(SWITCHING_NOZZLE) && !SWITCHING_NOZZLE_TWO_SERVOS   // Switching Spindle (single servo)
        // Raise by a configured distance to avoid workpiece, except with
        // SWITCHING_NOZZLE_TWO_SERVOS, as both tools will lift instead.
        if (!no_move) {
          const float newz = current_position.z + _MAX(-diff.z, 0.0);

          // Check if Z has space to compensate at least z_offset, and if not, just abort now
          const float maxz = _MIN(TERN(HAS_SOFTWARE_ENDSTOPS, soft_endstop.max.z, Z_MAX_POS), Z_MAX_POS);
          if (newz > maxz) return;

          current_position.z = _MIN(newz + toolchange_settings.z_raise, maxz);
          fast_line_to_current(Z_AXIS);
        }
        move_tool_servo(new_tool);
      #endif

      IF_DISABLED(DUAL_X_CARRIAGE, active_tool = new_tool); // Set the new active ATC tool

      TERN_(TOOL_SENSOR, tool_sensor_disabled = false);

      (void)check_tool_sensor_stats(active_tool, true);

      // The newly-selected ATC tool XYZ is actually at...
      DEBUG_ECHOLNPGM("Offset Tool XYZ by { ", diff.x, ", ", diff.y, ", ", diff.z, " }");
      current_position += diff;

      // Tell the planner the new "current position"
      sync_plan_position();

      #if ENABLED(DELTA)
        //LOOP_LINEAR_AXES(i) update_software_endstops(i); // or modify the constrain function
        const bool safe_to_move = current_position.z < delta_clip_start_height - 1;
      #else
        constexpr bool safe_to_move = true;
      #endif

      // Return to position and lower again
      const bool should_move = safe_to_move && !no_move && IsRunning();
      if (should_move) {

        #if EITHER(SINGLENOZZLE_STANDBY_TEMP, SINGLENOZZLE_STANDBY_FAN)
          fanManager.singletool_change(old_tool, new_tool);
        #endif

        #if ENABLED(TOOLCHANGE_FILAMENT_SWAP)
          if (should_swap && !too_cold) {

            float fr = toolchange_settings.unretract_speed;

            #if ENABLED(TOOLCHANGE_FS_INIT_BEFORE_SWAP)
              if (!toolchange_atc_tool_ready[new_tool]) {
                toolchange_atc_tool_ready[new_tool] = true;
                fr = toolchange_settings.prime_speed;       // Next move is a prime
                unscaled_e_move(0, MMM_TO_MMS(fr));         // Init planner with 0 length move
              }
            #endif

            // Unretract (or Prime)
            unscaled_e_move(toolchange_settings.swap_length, MMM_TO_MMS(fr));

            // Extra Prime
            unscaled_e_move(toolchange_settings.extra_prime, MMM_TO_MMS(toolchange_settings.prime_speed));

            // Cutting retraction
            #if TOOLCHANGE_FS_WIPE_RETRACT
              unscaled_e_move(-(TOOLCHANGE_FS_WIPE_RETRACT), MMM_TO_MMS(toolchange_settings.retract_speed));
            #endif

            // Cool down with fan
            #if HAS_FAN && TOOLCHANGE_FS_FAN >= 0
              fanManager.fan_speed[TOOLCHANGE_FS_FAN] = toolchange_settings.fan_speed;
              gcode.dwell(SEC_TO_MS(toolchange_settings.fan_time));
              fanManager.fan_speed[TOOLCHANGE_FS_FAN] = 0;
            #endif
          }
        #endif

        // Prevent a move outside physical bounds
        #if ENABLED(MAGNETIC_SWITCHING_TOOLHEAD)
          // If the original position is within tool store area, go to X origin at once
          if (destination.y < SWITCHING_TOOLHEAD_Y_POS + SWITCHING_TOOLHEAD_Y_CLEAR) {
            current_position.x = X_MIN_POS;
            planner.buffer_line(current_position, planner.settings.max_feedrate_mm_s[X_AXIS], new_tool);
            planner.synchronize();
          }
        #else
          apply_motion_limits(destination);
        #endif

        // Should the tool move back to the old position?
        if (can_move_away) {
          #if ENABLED(TOOLCHANGE_NO_RETURN)
            // Just move back down
            DEBUG_ECHOLNPGM("Move back Z only");

            if (TERN1(TOOLCHANGE_PARK, toolchange_settings.enable_park))
              do_blocking_move_to_z(destination.z, planner.settings.max_feedrate_mm_s[Z_AXIS]);

          #else
            // Move back to the original (or adjusted) position
            DEBUG_POS("Move back", destination);

            #if ENABLED(TOOLCHANGE_PARK)
              if (toolchange_settings.enable_park) do_blocking_move_to_xy_z(destination, destination.z, MMM_TO_MMS(TOOLCHANGE_PARK_XY_FEEDRATE));
            #else
              do_blocking_move_to_xy(destination, planner.settings.max_feedrate_mm_s[X_AXIS]);
              do_blocking_move_to_z(destination.z, planner.settings.max_feedrate_mm_s[Z_AXIS]);
            #endif

          #endif
        }

        else DEBUG_ECHOLNPGM("Move back skipped");

        #if ENABLED(TOOLCHANGE_FILAMENT_SWAP)
          if (should_swap && !too_cold) {
            // Cutting recover
            unscaled_e_move(toolchange_settings.extra_resume + TOOLCHANGE_FS_WIPE_RETRACT, MMM_TO_MMS(toolchange_settings.unretract_speed));
            current_position.e = 0;
            sync_plan_position_e(); // New ATC tool primed and set to 0

            // Restart Fan
            #if HAS_FAN && TOOLCHANGE_FS_FAN >= 0
              RESTORE(fan);
            #endif
          }
        #endif

        TERN_(DUAL_X_CARRIAGE, idex_set_parked(false));
      }

      #if ENABLED(SWITCHING_NOZZLE)
        // Move back down. (Including when the new tool is higher.)
        if (!should_move)
          do_blocking_move_to_z(destination.z, planner.settings.max_feedrate_mm_s[Z_AXIS]);
      #endif

      TERN_(SWITCHING_NOZZLE_TWO_SERVOS, lower_tool(new_tool));

    } // (new_tool != old_tool)

    planner.synchronize();

    #if ENABLED(EXT_SOLENOID) && DISABLED(PARKING_EXTRUDER)
      disable_all_solenoids();
      enable_solenoid_on_active_tool();
    #endif

    #if DO_SWITCH_EXTRUDER
      planner.synchronize();
      move_atc_tool_servo(active_tool);
    #endif

    TERN_(HAS_FANMUX, fanmux_switch(active_tool));

    if (ENABLED(EVENT_GCODE_TOOLCHANGE_ALWAYS_RUN) || !no_move) {
      #ifdef EVENT_GCODE_TOOLCHANGE_T0
        if (new_tool == 0)
          gcode.process_subcommands_now(F(EVENT_GCODE_TOOLCHANGE_T0));
      #endif

      #ifdef EVENT_GCODE_TOOLCHANGE_T1
        if (new_tool == 1)
          gcode.process_subcommands_now(F(EVENT_GCODE_TOOLCHANGE_T1));
      #endif

      #ifdef EVENT_GCODE_AFTER_TOOLCHANGE
        if (TERN1(DUAL_X_CARRIAGE, dual_x_carriage_mode == DXC_AUTO_PARK_MODE))
          gcode.process_subcommands_now(F(EVENT_GCODE_AFTER_TOOLCHANGE));
      #endif
    }

    SERIAL_ECHO_MSG(STR_ACTIVE_EXTRUDER, active_tool);

  #endif // TOOL_CHANGE_SUPPORT
}

#if ENABLED(TOOLCHANGE_MIGRATION_FEATURE)

  #define DEBUG_OUT ENABLED(DEBUG_TOOLCHANGE_MIGRATION_FEATURE)
  #include "../core/debug_out.h"

  bool atc_tool_migration() {

    // No auto-migration or specified target?
    if (!migration.target && active_tool >= migration.last) {
      DEBUG_ECHO_MSG("No Migration Target");
      DEBUG_ECHO_MSG("Target: ", migration.target, " Last: ", migration.last, " Active: ", active_tool);
      migration.automode = false;
      return false;
    }

    // Migrate to a target or the next ATC tool

    uint8_t migration_atc_tool = active_tool;

    if (migration.target) {
      DEBUG_ECHOLNPGM("Migration using fixed target");
      // Specified target ok?
      const int16_t t = migration.target - 1;
      if (t != active_tool) migration_atc_tool = t;
    }
    else if (migration.automode && migration_atc_tool < migration.last && migration_atc_tool < ATC_TOOLS - 1)
      migration_atc_tool++;

    if (migration_atc_tool == active_tool) {
      DEBUG_ECHOLNPGM("Migration source matches active");
      return false;
    }

    // Migration begins
    DEBUG_ECHOLNPGM("Beginning migration");

    migration.in_progress = true; // Prevent runout script
    planner.synchronize();

    // Perform the tool change
    tool_change(migration_atc_tool);

    // If no available ATC tool
    if (ATC_TOOLS < 2 || active_tool >= ATC_TOOLS - 2 || active_tool == migration.last)
      migration.automode = false;

    migration.in_progress = false;

    planner.synchronize();
    DEBUG_ECHOLNPGM("Migration Complete");
    return true;
  }

#endif // TOOLCHANGE_MIGRATION_FEATURE
