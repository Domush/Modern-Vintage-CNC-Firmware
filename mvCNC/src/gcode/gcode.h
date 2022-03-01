/**
 * Modern Vintage CNC Firmware
*/
#pragma once

/**
 * -----------------
 * G-Codes in mvCNC
 * -----------------
 *
 * Helpful G-code references:
 *  - https://Marlinfw.org/meta/gcode
 *  - https://reprap.org/wiki/G-code
 *  - https://linuxcnc.org/docs/html/gcode.html
 *
 * -----------------
 *
 * "G" Codes
 *
 *
 * "M" Codes
 *
 *** "T" Codes ***
 *
 * T0-T3 - Select an ATC tool (tool) by index: "T<n> F<units/min>"
 */

#include "../inc/mvCNCConfig.h"
#include "parser.h"

#if ENABLED(I2C_POSITION_ENCODERS)
  #include "../feature/encoder_i2c.h"
#endif

#if defined(G0_FEEDRATE)
  #define HAS_FAST_MOVES 1
#endif

enum AxisRelative : uint8_t { LOGICAL_AXIS_LIST(REL_E, REL_X, REL_Y, REL_Z, REL_I, REL_J, REL_K) };

extern const char G28_STR[];

class GcodeSuite {
public:
  static axis_bits_t axis_relative;

  static bool axis_is_relative(const AxisEnum a) { return TEST(axis_relative, a); }
  static void set_relative_mode(const bool rel) {
    axis_relative = rel ? (0 LOGICAL_AXIS_GANG(| _BV(REL_E), | _BV(REL_X), | _BV(REL_Y), | _BV(REL_Z), | _BV(REL_I),
                                               | _BV(REL_J), | _BV(REL_K)))
                        : 0;
  }

#if ENABLED(CNC_WORKSPACE_PLANES)
  /**
     * Workspace planes only apply to G2/G3 moves
     * (and "canned cycles" - not a current feature)
     */
  enum WorkspacePlane : char {
    PLANE_XY,
    PLANE_ZX,
    PLANE_YZ
  };
  static WorkspacePlane workspace_plane;
#endif

#define MAX_COORDINATE_SYSTEMS 9
#if ENABLED(CNC_COORDINATE_SYSTEMS)
  static int8_t active_coordinate_system;
  static xyz_pos_t coordinate_system[MAX_COORDINATE_SYSTEMS];
  static bool select_coordinate_system(const int8_t _new);
#endif

  static millis_t previous_move_ms, max_inactive_time, stepper_inactive_time;
  FORCE_INLINE static void reset_stepper_timeout(const millis_t ms = millis()) { previous_move_ms = ms; }
  FORCE_INLINE static bool stepper_max_timed_out(const millis_t ms = millis()) {
    return max_inactive_time && ELAPSED(ms, previous_move_ms + max_inactive_time);
  }
  FORCE_INLINE static bool stepper_inactive_timeout(const millis_t ms = millis()) {
    return ELAPSED(ms, previous_move_ms + stepper_inactive_time);
  }

  static void report_echo_start(const bool forReplay);
  static void report_heading(const bool forReplay, FSTR_P const fstr, const bool eol = true);
  static void report_heading_etc(const bool forReplay, FSTR_P const fstr, const bool eol = true) {
    report_heading(forReplay, fstr, eol);
    report_echo_start(forReplay);
  }
  static void say_units();

  static int8_t get_tool_from_command();
  static void get_destination_from_command();

  static void process_parsed_command(const bool no_ok = false);
  static void process_next_command();

  // Execute G-code in-place, preserving current G-code parameters
  static void process_subcommands_now(FSTR_P fgcode, bool no_ok = true);
  static void process_subcommands_now(char *gcode, bool no_ok = true);

  static void home_all_axes(const bool keep_leveling = false) {
    process_subcommands_now(keep_leveling ? FPSTR(G28_STR)
                                          : TERN(CAN_SET_LEVELING_AFTER_G28, F("G28L0"), FPSTR(G28_STR)));
  }

#if EITHER(HAS_AUTO_REPORTING, HOST_KEEPALIVE_FEATURE)
  static bool autoreport_paused;
  static bool set_autoreport_paused(const bool p) {
    const bool was    = autoreport_paused;
    autoreport_paused = p;
    return was;
  }
#else
  static constexpr bool autoreport_paused = false;
  static bool set_autoreport_paused(const bool) { return false; }
#endif

#if ENABLED(HOST_KEEPALIVE_FEATURE)
  /**
     * States for managing mvCNC and host communication
     * mvCNC sends messages if blocked or busy
     */
  enum mvCNCBusyState : char {
    NOT_BUSY,         // Not in a handler
    IN_HANDLER,       // Processing a GCode
    IN_PROCESS,       // Known to be blocking command input (as in G29)
    PAUSED_FOR_USER,  // Blocking pending any input
    PAUSED_FOR_INPUT  // Blocking pending text input (concept)
  };

  static mvCNCBusyState busy_state;
  static uint8_t host_keepalive_interval;

  static void host_keepalive();
  static bool host_keepalive_is_paused() { return busy_state >= PAUSED_FOR_USER; }

  #define KEEPALIVE_STATE(N) REMEMBER(_KA_, gcode.busy_state, gcode.N)
#else
  #define KEEPALIVE_STATE(N) NOOP
#endif

  static void dwell(millis_t time);

private:
  friend class mvCNCSettings;

#if ENABLED(MVCNC_DEV_MODE)
  static void D(const int16_t dcode);
#endif

  static void G0_G1(TERN_(HAS_FAST_MOVES, const bool fast_move = false));

#if ENABLED(ARC_SUPPORT)
  static void G2_G3(const bool clockwise);
#endif

  static void G4();

#if ENABLED(BEZIER_CURVE_SUPPORT)
  static void G5();
#endif

#if ENABLED(DIRECT_STEPPING)
  static void G6();
#endif

#if ENABLED(CNC_COORDINATE_SYSTEMS)
  static void G10();
#endif

#if ENABLED(CNC_WORKSPACE_PLANES)
  static void G17();
  static void G18();
  static void G19();
#endif

#if ENABLED(INCH_MODE_SUPPORT)
  static void G20();
  static void G21();
#endif

#if ENABLED(SPINDLE_PARK_FEATURE)
  static void G27();
#endif

  static void G28();

#if HAS_BED_PROBE
  static void G30();
  #if ENABLED(Z_PROBE_SLED)
  static void G31();
  static void G32();
  #endif
#endif

#if ENABLED(DELTA_AUTO_CALIBRATION)
  static void G33();
#endif

#if ANY(Z_MULTI_ENDSTOPS, Z_STEPPER_AUTO_ALIGN, MECHANICAL_GANTRY_CALIBRATION)
  static void G34();
#endif

#if ENABLED(Z_STEPPER_AUTO_ALIGN)
  static void M422();
  static void M422_report(const bool forReplay = true);
#endif

#if ENABLED(ASSISTED_TRAMMING)
  static void G35();
#endif

#if ENABLED(G38_PROBE_TARGET)
  static void G38(const int8_t subcode);
#endif

#if ENABLED(CNC_COORDINATE_SYSTEMS)
  static void G53();
  static void G54();
  static void G55();
  static void G56();
  static void G57();
  static void G58();
  static void G59();
#endif

#if SAVED_POSITIONS
  static void G60();
  static void G61();
#endif

#if ENABLED(GCODE_MOTION_MODES)
  static void G80();
#endif

  static void G92();

#if HAS_RESUME_CONTINUE
  static void M0_M1();
#endif

#if HAS_CUTTER
  static void M3_M4(const bool is_M4);
  static void M5();
#endif

#if ENABLED(COOLANT_MIST)
  static void M7();
#endif

#if EITHER(AIR_ASSIST, COOLANT_FLOOD)
  static void M8();
#endif

#if EITHER(AIR_ASSIST, COOLANT_CONTROL)
  static void M9();
#endif

#if ENABLED(AIR_EVACUATION)
  static void M10();
  static void M11();
#endif

#if ENABLED(EXTERNAL_CLOSED_LOOP_CONTROLLER)
  static void M12();
#endif

#if ENABLED(CNC_ID_CHECK)
  static void M16();
#endif

  static void M17();

  static void M18_M84();

#if ENABLED(SDSUPPORT)
  static void M20();
  static void M21();
  static void M22();
  static void M23();
  static void M24();
  static void M25();
  static void M26();
  static void M27();
  static void M28();
  static void M29();
  static void M30();
#endif

  static void M31();

#if ENABLED(SDSUPPORT)
  #if HAS_MEDIA_SUBCALLS
  static void M32();
  #endif
  #if ENABLED(LONG_FILENAME_HOST_SUPPORT)
  static void M33();
  #endif
  #if BOTH(SDCARD_SORT_ALPHA, SDSORT_GCODE)
  static void M34();
  #endif
#endif

#if ENABLED(DIRECT_PIN_CONTROL)
  static void M42();
#endif
#if ENABLED(PINS_DEBUGGING)
  static void M43();
#endif

#if ENABLED(Z_MIN_PROBE_REPEATABILITY_TEST)
  static void M48();
#endif

#if ENABLED(LCD_SET_PROGRESS_MANUALLY)
  static void M73();
#endif

  static void M75();
  static void M76();
  static void M77();

#if ENABLED(JOBCOUNTER)
  static void M78();
#endif

#if ENABLED(PSU_CONTROL)
  static void M80();
#endif
  static void M81();

  static void M85();
  static void M92();
  static void M92_report(const bool forReplay = true, const int8_t e = -1);

#if ENABLED(M100_FREE_MEMORY_WATCHER)
  static void M100();
#endif

  static void M105();

#if HAS_FAN
  static void M106();
  static void M107();
#endif

#if DISABLED(EMERGENCY_PARSER)
  static void M108();
  static void M112();
  static void M410();
  #if ENABLED(HOST_PROMPT_SUPPORT)
  static void M876();
  #endif
#endif

  static void M110();
  static void M111();

#if ENABLED(HOST_KEEPALIVE_FEATURE)
  static void M113();
#endif

  static void M114();
  static void M115();

#if HAS_STATUS_MESSAGE
  static void M117();
#endif

  static void M118();
  static void M119();
  static void M120();
  static void M121();

#if HAS_FANCHECK
  static void M123();
#endif

#if ENABLED(PARK_HEAD_ON_PAUSE)
  static void M125();
#endif

#if HAS_COOLER
  static void M143();
  static void M193();
#endif

#if ENABLED(TEMPERATURE_UNITS_SUPPORT)
  static void M149();
  static void M149_report(const bool forReplay = true);
#endif

#if HAS_COLOR_LEDS
  static void M150();
#endif

#if ENABLED(AUTO_REPORT_POSITION)
  static void M154();
#endif

  static void M201();
  static void M201_report(const bool forReplay = true);

#if 0
    static void M202(); // Not used for Marlin/grbl gen6
#endif

  static void M203();
  static void M203_report(const bool forReplay = true);
  static void M204();
  static void M204_report(const bool forReplay = true);
  static void M205();
  static void M205_report(const bool forReplay = true);

#if HAS_M206_COMMAND
  static void M206();
  static void M206_report(const bool forReplay = true);
#endif

  static void M211();
  static void M211_report(const bool forReplay = true);

  #if TOOL_CHANGE_SUPPORT
    static void M217();
    static void M217_report(const bool forReplay=true);
  #endif


  static void M220();

#if ENABLED(DIRECT_PIN_CONTROL)
  static void M226();
#endif

#if ENABLED(PHOTO_GCODE)
  static void M240();
#endif

#if HAS_LCD_CONTRAST
  static void M250();
  static void M250_report(const bool forReplay = true);
#endif

#if HAS_LCD_BRIGHTNESS
  static void M256();
  static void M256_report(const bool forReplay = true);
#endif

#if ENABLED(EXPERIMENTAL_I2CBUS)
  static void M260();
  static void M261();
#endif

#if HAS_SERVOS
  static void M280();
  #if ENABLED(EDITABLE_SERVO_ANGLES)
  static void M281();
  static void M281_report(const bool forReplay = true);
  #endif
  #if ENABLED(SERVO_DETACH_GCODE)
  static void M282();
  #endif
#endif

#if ENABLED(BABYSTEPPING)
  static void M290();
#endif

#if HAS_BUZZER
  static void M300();
#endif

#if HAS_USER_THERMISTORS
  static void M305();
#endif

#if HAS_MICROSTEPS
  static void M350();
  static void M351();
#endif

#if ENABLED(CASE_LIGHT_ENABLE)
  static void M355();
#endif

#if EITHER(EXT_SOLENOID, MANUAL_SOLENOID_CONTROL)
  static void M380();
  static void M381();
#endif

  static void M400();

#if HAS_BED_PROBE
  static void M401();
  static void M402();
#endif

#if HAS_MULTI_LANGUAGE
  static void M414();
  static void M414_report(const bool forReplay = true);
#endif

#if ENABLED(BACKLASH_GCODE)
  static void M425();
  static void M425_report(const bool forReplay = true);
#endif

#if HAS_M206_COMMAND
  static void M428();
#endif

#if HAS_POWER_MONITOR
  static void M430();
#endif

  static void M500();
  static void M501();
  static void M502();
#if DISABLED(DISABLE_M503)
  static void M503();
#endif
#if ENABLED(EEPROM_SETTINGS)
  static void M504();
#endif

#if ENABLED(PASSWORD_FEATURE)
  static void M510();
  #if ENABLED(PASSWORD_UNLOCK_GCODE)
  static void M511();
  #endif
  #if ENABLED(PASSWORD_CHANGE_GCODE)
  static void M512();
  #endif
#endif

#if ENABLED(SDSUPPORT)
  static void M524();
#endif

#if ENABLED(SD_ABORT_ON_ENDSTOP_HIT)
  static void M540();
#endif

#if HAS_ETHERNET
  static void M552();
  static void M552_report();
  static void M553();
  static void M553_report();
  static void M554();
  static void M554_report();
#endif

#if HAS_STEALTHCHOP
  static void M569();
  static void M569_report(const bool forReplay = true);
#endif

#if ENABLED(BAUD_RATE_GCODE)
  static void M575();
#endif

#if HAS_DUPLICATION_MODE
  static void M605();
#endif

#if EITHER(DELTA, HAS_EXTRA_ENDSTOPS)
  static void M666();
  static void M666_report(const bool forReplay = true);
#endif

#if ENABLED(GCODE_REPEAT_MARKERS)
  static void M808();
#endif

#if ENABLED(GCODE_MACROS)
  static void M810_819();
#endif

#if HAS_BED_PROBE
  static void M851();
  static void M851_report(const bool forReplay = true);
#endif

#if ENABLED(SKEW_CORRECTION_GCODE)
  static void M852();
  static void M852_report(const bool forReplay = true);
#endif

#if ENABLED(I2C_POSITION_ENCODERS)
  FORCE_INLINE static void M860() { I2CPEM.M860(); }
  FORCE_INLINE static void M861() { I2CPEM.M861(); }
  FORCE_INLINE static void M862() { I2CPEM.M862(); }
  FORCE_INLINE static void M863() { I2CPEM.M863(); }
  FORCE_INLINE static void M864() { I2CPEM.M864(); }
  FORCE_INLINE static void M865() { I2CPEM.M865(); }
  FORCE_INLINE static void M866() { I2CPEM.M866(); }
  FORCE_INLINE static void M867() { I2CPEM.M867(); }
  FORCE_INLINE static void M868() { I2CPEM.M868(); }
  FORCE_INLINE static void M869() { I2CPEM.M869(); }
#endif

#if HAS_PTC
  static void M871();
#endif

#if HAS_TRINAMIC_CONFIG
  static void M122();
  static void M906();
  static void M906_report(const bool forReplay = true);
  #if ENABLED(MONITOR_DRIVER_STATUS)
  static void M911();
  static void M912();
  #endif
  #if ENABLED(HYBRID_THRESHOLD)
  static void M913();
  static void M913_report(const bool forReplay = true);
  #endif
  #if USE_SENSORLESS
  static void M914();
  static void M914_report(const bool forReplay = true);
  #endif
  static void M919();
#endif

#if HAS_MOTOR_CURRENT_SPI || HAS_MOTOR_CURRENT_PWM || HAS_MOTOR_CURRENT_I2C || HAS_MOTOR_CURRENT_DAC
  static void M907();
  #if HAS_MOTOR_CURRENT_SPI || HAS_MOTOR_CURRENT_PWM
  static void M907_report(const bool forReplay = true);
  #endif
#endif
#if HAS_MOTOR_CURRENT_SPI || HAS_MOTOR_CURRENT_DAC
  static void M908();
#endif
#if HAS_MOTOR_CURRENT_DAC
  static void M909();
  static void M910();
#endif

#if ENABLED(SDSUPPORT)
  static void M928();
#endif

#if ENABLED(MAGNETIC_PARKING_EXTRUDER)
  static void M951();
#endif

#if ENABLED(TOUCH_SCREEN_CALIBRATION)
  static void M995();
#endif

#if BOTH(HAS_SPI_FLASH, SDSUPPORT)
  static void M993();
  static void M994();
#endif

#if ENABLED(PLATFORM_M997_SUPPORT)
  static void M997();
#endif

  static void M999();

#if ENABLED(POWER_LOSS_RECOVERY)
  static void M413();
  static void M413_report(const bool forReplay = true);
  static void M1000();
#endif

#if ENABLED(SDSUPPORT)
  static void M1001();
#endif

#if ENABLED(DGUS_LCD_UI_MKS)
  static void M1002();
#endif

#if ENABLED(HAS_MCP3426_ADC)
  static void M3426();
#endif

#if ENABLED(MAX7219_GCODE)
  static void M7219();
#endif

#if ENABLED(CONTROLLER_FAN_EDITABLE)
  static void M710();
  static void M710_report(const bool forReplay = true);
#endif

  static void T(const int8_t tool_index);
};

extern GcodeSuite gcode;
