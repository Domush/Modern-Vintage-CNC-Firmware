/**
 * Webber Ranch CNC Firmware
*/
#pragma once

/**
 * feature/powerloss.h - Resume an SD print after power-loss
 */

#include "../sd/cardreader.h"
#include "../gcode/gcode.h"

#include "../inc/WRCNCConfig.h"

#if ENABLED(GCODE_REPEAT_MARKERS)
  #include "../feature/repeat.h"
#endif

#if ENABLED(MIXING_EXTRUDER)
  #include "../feature/mixing.h"
#endif

#if !defined(POWER_LOSS_STATE) && PIN_EXISTS(POWER_LOSS)
  #define POWER_LOSS_STATE HIGH
#endif

#ifndef POWER_LOSS_ZRAISE
  #define POWER_LOSS_ZRAISE 2
#endif

//#define DEBUG_POWER_LOSS_RECOVERY
//#define SAVE_EACH_CMD_MODE
//#define SAVE_INFO_INTERVAL_MS 0

typedef struct {
  uint8_t valid_head;

  // Machine state
  xyze_pos_t current_position;
  uint16_t feedrate;

  float zraise;

  // Repeat information
  #if ENABLED(GCODE_REPEAT_MARKERS)
    Repeat stored_repeat;
  #endif

  #if HAS_HOME_OFFSET
    xyz_pos_t home_offset;
  #endif
  #if HAS_POSITION_SHIFT
    xyz_pos_t position_shift;
  #endif
  #if HAS_MULTI_EXTRUDER
    uint8_t active_extruder;
  #endif

  #if DISABLED(NO_VOLUMETRICS)
    float filament_size[EXTRUDERS];
  #endif

  #if HAS_HOTEND
    celsius_t target_temperature[HOTENDS];
  #endif
  #if HAS_HEATED_BED
    celsius_t target_temperature_bed;
  #endif
  #if HAS_FAN
    uint8_t fan_speed[FAN_COUNT];
  #endif

  #if HAS_LEVELING
    float fade;
  #endif

  #if ENABLED(FWRETRACT)
    float retract[EXTRUDERS], retract_hop;
  #endif

  // Mixing extruder and gradient
  #if ENABLED(MIXING_EXTRUDER)
    //uint_fast8_t selected_vtool;
    //mixer_comp_t color[NR_MIXING_VIRTUAL_TOOLS][MIXING_STEPPERS];
    #if ENABLED(GRADIENT_MIX)
      gradient_t gradient;
    #endif
  #endif

  // SD Filename and position
  char sd_filename[MAXPATHNAMELENGTH];
  volatile uint32_t sdpos;

  // Job elapsed time
  millis_t print_job_elapsed;

  // Relative axis modes
  uint8_t axis_relative;

  // Misc. WRCNC flags
  struct {
    bool raised:1;                // Raised before saved
    bool dryrun:1;                // M111 S8
    bool allow_cold_extrusion:1;  // M302 P1
    #if HAS_LEVELING
      bool leveling:1;            // M420 S
    #endif
    #if DISABLED(NO_VOLUMETRICS)
      bool volumetric_enabled:1;  // M200 S D
    #endif
  } flag;

  uint8_t valid_foot;

  bool valid() { return valid_head && valid_head == valid_foot; }

} job_recovery_info_t;

class CNCJobRecovery {
  public:
    static const char filename[5];

    static SdFile file;
    static job_recovery_info_t info;

    static uint8_t queue_index_r;     //!< Queue index of the active command
    static uint32_t cmd_sdpos,        //!< SD position of the next command
                    sdpos[BUFSIZE];   //!< SD positions of queued commands

    #if HAS_DWIN_E3V2_BASIC
      static bool dwin_flag;
    #endif

    static void init();
    static void prepare();

    static void setup() {
      #if PIN_EXISTS(POWER_LOSS)
        #if ENABLED(POWER_LOSS_PULLUP)
          SET_INPUT_PULLUP(POWER_LOSS_PIN);
        #elif ENABLED(POWER_LOSS_PULLDOWN)
          SET_INPUT_PULLDOWN(POWER_LOSS_PIN);
        #else
          SET_INPUT(POWER_LOSS_PIN);
        #endif
      #endif
    }

    // Track each command's file offsets
    static uint32_t command_sdpos() { return sdpos[queue_index_r]; }
    static void commit_sdpos(const uint8_t index_w) { sdpos[index_w] = cmd_sdpos; }

    static bool enabled;
    static void enable(const bool onoff);
    static void changed();

    static bool exists() { return card.jobRecoverFileExists(); }
    static void open(const bool read) { card.openJobRecoveryFile(read); }
    static void close() { file.close(); }

    static void check();
    static void resume();
    static void purge();

    static void cancel() { purge(); IF_DISABLED(NO_SD_AUTOSTART, card.autofile_begin()); }

    static void load();
    static void save(const bool force=ENABLED(SAVE_EACH_CMD_MODE), const float zraise=POWER_LOSS_ZRAISE, const bool raised=false);

    #if PIN_EXISTS(POWER_LOSS)
      static void outage() {
        static constexpr uint8_t OUTAGE_THRESHOLD = 3;
        static uint8_t outage_counter = 0;
        if (enabled && READ(POWER_LOSS_PIN) == POWER_LOSS_STATE) {
          outage_counter++;
          if (outage_counter >= OUTAGE_THRESHOLD) _outage();
        }
        else
          outage_counter = 0;
      }
    #endif

    // The referenced file exists
    static bool interrupted_file_exists() { return card.fileExists(info.sd_filename); }

    static bool valid() { return info.valid() && interrupted_file_exists(); }

    #if ENABLED(DEBUG_POWER_LOSS_RECOVERY)
      static void debug(FSTR_P const prefix);
    #else
      static void debug(FSTR_P const) {}
    #endif

  private:
    static void write();

    #if ENABLED(BACKUP_POWER_SUPPLY)
      static void retract_and_lift(const_float_t zraise);
    #endif

    #if PIN_EXISTS(POWER_LOSS)
      friend class GcodeSuite;
      static void _outage();
    #endif
};

extern CNCJobRecovery recovery;