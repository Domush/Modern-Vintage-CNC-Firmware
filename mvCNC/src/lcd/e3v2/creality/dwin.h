/**
 * Modern Vintage CNC Firmware
*/
#pragma once

/**
 * DWIN by Creality3D
 */

#include "dwin_lcd.h"
#include "../common/encoder.h"
#include "../../../libs/BL24CXX.h"

#include "../../../inc/mvCNCConfigPre.h"

enum processID : uint8_t {
  // Process ID
  MainMenu,
  SelectFile,
  Prepare,
  Control,
  Leveling,
  CNCProcess,
  AxisMove,
  TemperatureID,
  Motion,
  Info,
  Tune,
  #if HAS_PREHEAT
    PLAPreheat,
    #if PREHEAT_COUNT > 1
      ABSPreheat,
    #endif
  #endif
  MaxSpeed,
  MaxSpeed_value,
  MaxAcceleration,
  MaxAcceleration_value,
  MaxJerk,
  MaxJerk_value,
  Step,
  Step_value,
  HomeOff,
  HomeOffX,
  HomeOffY,
  HomeOffZ,

  // Last Process ID
  Last_Prepare,

  // Advance Settings
  AdvSet,
  ProbeOff,
  ProbeOffX,
  ProbeOffY,

  // Back Process ID
  Back_Main,
  Back_Print,

  // Date variable ID
  Move_X,
  Move_Y,
  Move_Z,
  #if HAS_HOTEND
    Extruder,
    ETemp,
  #endif
  Homeoffset,
  #if HAS_HEATED_BED
    BedTemp,
  #endif
  #if HAS_FAN
    FanSpeed,
  #endif
  CNCSpeed,

  // Window ID
  CNC_window,
  Popup_Window
};

extern uint8_t checkkey;
extern float zprobe_zoffset;
extern char print_filename[16];

extern millis_t dwin_heat_time;

typedef struct {
  #if HAS_HOTEND
    celsius_t E_Temp = 0;
  #endif
  #if HAS_HEATED_BED
    celsius_t Bed_Temp = 0;
  #endif
  #if HAS_FAN
    int16_t Fan_speed = 0;
  #endif
  int16_t print_speed     = 100;
  float Max_Feedspeed     = 0;
  float Max_Acceleration  = 0;
  float Max_Jerk_scaled   = 0;
  float Max_Step_scaled   = 0;
  float Move_X_scaled     = 0;
  float Move_Y_scaled     = 0;
  float Move_Z_scaled     = 0;
  #if HAS_HOTEND
    float Move_E_scaled   = 0;
  #endif
  float offset_value      = 0;
  int8_t show_mode        = 0; // -1: Temperature control    0: CNCing temperature
  float Home_OffX_scaled  = 0;
  float Home_OffY_scaled  = 0;
  float Home_OffZ_scaled  = 0;
  float Probe_OffX_scaled = 0;
  float Probe_OffY_scaled = 0;
} HMI_value_t;

#define DWIN_CHINESE 123
#define DWIN_ENGLISH 0

typedef struct {
  uint8_t language;
  bool pause_flag:1;    // cutting is paused
  bool pause_action:1;  // flag a pause action
  bool print_finish:1;  // print was finished
  bool select_flag:1;   // Popup button selected
  bool home_flag:1;     // homing in course
  bool heat_flag:1;     // 0: heating done  1: during heating
  bool done_confirm_flag:1;
  #if ENABLED(PREVENT_COLD_EXTRUSION)
    bool cold_flag:1;
  #endif
  AxisEnum feedspeed_axis, acc_axis, jerk_axis, step_axis;
} HMI_flag_t;

extern HMI_value_t HMI_ValueStruct;
extern HMI_flag_t HMI_flag;

#if HAS_HOTEND || HAS_HEATED_BED
  // Popup message window
  void DWIN_Popup_Temperature(const bool toohigh);
#endif

#if HAS_HOTEND
  void Popup_Window_ETempTooLow();
#endif

void Popup_Window_Resume();
void Popup_Window_Home(const bool parking=false);
void Popup_Window_Leveling();

void Goto_PrintProcess();
void Goto_MainMenu();

// Variable control
void HMI_Move_X();
void HMI_Move_Y();
void HMI_Move_Z();
void HMI_Move_E();

void HMI_Zoffset();

#if HAS_HOTEND
  void HMI_ETemp();
#endif
#if HAS_HEATED_BED
  void HMI_BedTemp();
#endif
#if HAS_FAN
  void HMI_FanSpeed();
#endif

void HMI_PrintSpeed();

void HMI_MaxFeedspeedXYZE();
void HMI_MaxAccelerationXYZE();
void HMI_MaxJerkXYZE();
void HMI_StepXYZE();
void HMI_SetLanguageCache();

void update_variable();
void DWIN_Draw_Signed_Float(uint8_t size, uint16_t bColor, uint8_t iNum, uint8_t fNum, uint16_t x, uint16_t y, long value);

// SD Card
void HMI_SDCardInit();
void HMI_SDCardUpdate();

// Main Process
void Icon_print(bool value);
void Icon_control(bool value);
void Icon_temperature(bool value);
void Icon_leveling(bool value);

// Other
void Draw_Status_Area(const bool with_update); // Status Area
void HMI_StartFrame(const bool with_update);   // Prepare the menu view
void HMI_MainMenu();    // Main process screen
void HMI_SelectFile();  // File page
void HMI_Cutting();    // CNC page
void HMI_Prepare();     // Prepare page
void HMI_Control();     // Control page
void HMI_Leveling();    // Level the page
void HMI_AxisMove();    // Axis movement menu
void HMI_Temperature(); // Temperature menu
void HMI_Motion();      // Sports menu
void HMI_Info();        // Information menu
void HMI_Tune();        // Adjust the menu

#if HAS_PREHEAT
  void HMI_PLAPreheatSetting(); // PLA warm-up setting
  void HMI_ABSPreheatSetting(); // ABS warm-up setting
#endif

void HMI_MaxSpeed();        // Maximum speed submenu
void HMI_MaxAcceleration(); // Maximum acceleration submenu
void HMI_MaxJerk();         // Maximum jerk speed submenu
void HMI_Step();            // Transmission ratio

void HMI_Init();
void DWIN_Update();
void EachMomentUpdate();
void DWIN_HandleScreen();
void DWIN_StatusChanged(const char * const cstr=nullptr);
void DWIN_StatusChanged(FSTR_P const fstr);

inline void DWIN_StartHoming() { HMI_flag.home_flag = true; }

void DWIN_CompletedHoming();
void DWIN_CompletedLeveling();
