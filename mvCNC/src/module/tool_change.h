/**
 * Modern Vintage CNC Firmware
*/
#pragma once

#include "../inc/mvCNCConfig.h"

//#define DEBUG_TOOLCHANGE_MIGRATION_FEATURE

#if TOOL_CHANGE_SUPPORT

  typedef struct {
    #if ENABLED(TOOLCHANGE_FILAMENT_SWAP)
      float swap_length, extra_prime, extra_resume;
      int16_t prime_speed, retract_speed, unretract_speed, fan, fan_speed, fan_time;
    #endif
    #if ENABLED(TOOLCHANGE_PARK)
      bool enable_park;
      xy_pos_t change_point;
    #endif
    float z_raise;
  } toolchange_settings_t;

  extern toolchange_settings_t toolchange_settings;

  #if ENABLED(TOOLCHANGE_FILAMENT_SWAP)
    void tool_change_prime();
  #endif

  #if ENABLED(TOOLCHANGE_FS_PRIME_FIRST_USED)
    extern bool enable_first_prime;
  #endif

  #if ENABLED(TOOLCHANGE_FS_INIT_BEFORE_SWAP)
    extern bool toolchange_atc_tool_ready[ATC_TOOLS];
  #endif

  #if ENABLED(TOOLCHANGE_MIGRATION_FEATURE)
    typedef struct {
      uint8_t target, last;
      bool automode, in_progress;
    } migration_settings_t;
    constexpr migration_settings_t migration_defaults = { 0, 0, false, false };
    extern migration_settings_t migration;
    bool atc_tool_migration();
  #endif
#endif

#if DO_SWITCH_EXTRUDER
  void move_atc_tool_servo(const uint8_t e);
#endif

#if ENABLED(SWITCHING_NOZZLE)
  #if SWITCHING_NOZZLE_TWO_SERVOS
    void lower_tool(const uint8_t e);
    void raise_tool(const uint8_t e);
  #else
    void move_tool_servo(const uint8_t angle_index);
  #endif
#endif

#if ENABLED(PARKING_EXTRUDER)

  void pe_solenoid_set_pin_state(const uint8_t atc_tool_num, const uint8_t state);

  #define PE_MAGNET_ON_STATE TERN_(PARKING_EXTRUDER_SOLENOIDS_INVERT, !)PARKING_EXTRUDER_SOLENOIDS_PINS_ACTIVE
  inline void pe_solenoid_magnet_on(const uint8_t atc_tool_num)  { pe_solenoid_set_pin_state(atc_tool_num,  PE_MAGNET_ON_STATE); }
  inline void pe_solenoid_magnet_off(const uint8_t atc_tool_num) { pe_solenoid_set_pin_state(atc_tool_num, !PE_MAGNET_ON_STATE); }

  void pe_solenoid_init();

  extern bool atc_tool_parked;
  inline void parking_atc_tool_set_parked(const bool parked) { atc_tool_parked = parked; }
  bool parking_atc_tool_unpark_after_homing(const uint8_t final_tool, bool homed_towards_final_tool);

#elif ENABLED(MAGNETIC_PARKING_EXTRUDER)

  typedef struct MPESettings {
      float parking_xpos[2],      // M951 L R
            grab_distance;        // M951 I
 feedRate_t slow_feedrate,        // M951 J
            fast_feedrate;        // M951 H
      float travel_distance,      // M951 D
            compensation_factor;  // M951 C
  } mpe_settings_t;

  extern mpe_settings_t mpe_settings;

  void mpe_settings_init();

#endif

#if ENABLED(ELECTROMAGNETIC_SWITCHING_TOOLHEAD)
  void est_init();
#elif ENABLED(SWITCHING_TOOLHEAD)
  void swt_init();
#endif

#if ENABLED(TOOL_SENSOR)
  uint8_t check_tool_sensor_stats(const uint8_t active_tool, const bool kill_on_error=false, const bool disable=false);
#else
  inline uint8_t check_tool_sensor_stats(const uint8_t, const bool=false, const bool=false) { return 0; }
#endif

/**
 * Perform a tool-change, which may result in moving the
 * previous tool out of the way and the new tool into place.
 */
void tool_change(const uint8_t tmp_atc_tool, bool no_move=false);
