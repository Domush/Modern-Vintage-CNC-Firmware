/**
 * Modern Vintage CNC Firmware
*/
#pragma once

/**
 * Croatian (Hrvatski)
 *
 * LCD Menu Messages
 * See also https://mvcncfw.org/docs/development/lcd_language.html
 */

#define DISPLAY_CHARSET_ISO10646_1 // use the better font on full graphic displays.

namespace Language_hr {
  using namespace Language_en; // Inherit undefined strings from English

  constexpr uint8_t CHARSIZE              = 2;
  LSTR LANGUAGE                           = _UxGT("Croatian");

  LSTR WELCOME_MSG                        = MACHINE_NAME _UxGT(" spreman.");
  LSTR MSG_MEDIA_INSERTED                 = _UxGT("SD kartica umetnuta");
  LSTR MSG_MEDIA_REMOVED                  = _UxGT("SD kartica uklonjena");
  LSTR MSG_LCD_ENDSTOPS                   = _UxGT("Endstops"); // Max length 8 characters
  LSTR MSG_MAIN                           = _UxGT("Main");
  LSTR MSG_RUN_AUTO_FILES                 = _UxGT("Auto pokretanje");
  LSTR MSG_DISABLE_STEPPERS               = _UxGT("Ugasi steppere");
  LSTR MSG_AUTO_HOME                      = _UxGT("Automatski homing");
  LSTR MSG_AUTO_HOME_X                    = _UxGT("Home-aj X");
  LSTR MSG_AUTO_HOME_Y                    = _UxGT("Home-aj Y");
  LSTR MSG_AUTO_HOME_Z                    = _UxGT("Home-aj Z");
  LSTR MSG_LEVEL_BED_HOMING               = _UxGT("Home-aj XYZ");
  LSTR MSG_LEVEL_BED_WAITING              = _UxGT("Klikni za početak");
  LSTR MSG_LEVEL_BED_NEXT_POINT           = _UxGT("Sljedeća točka");
  LSTR MSG_LEVEL_BED_DONE                 = _UxGT("Niveliranje gotovo!");
  LSTR MSG_SET_HOME_OFFSETS               = _UxGT("Postavi home offsete");
  LSTR MSG_HOME_OFFSETS_APPLIED           = _UxGT("Offsets postavljeni");
  LSTR MSG_SET_ORIGIN                     = _UxGT("Postavi ishodište");
  #if HAS_PREHEAT
    LSTR MSG_PREHEAT_1                    = _UxGT("Predgrij ") PREHEAT_1_LABEL;
    LSTR MSG_PREHEAT_1_H                  = _UxGT("Predgrij ") PREHEAT_1_LABEL " ~";
    LSTR MSG_PREHEAT_1_END                = _UxGT("Predgrij ") PREHEAT_1_LABEL _UxGT(" Dizna");
    LSTR MSG_PREHEAT_1_END_E              = _UxGT("Predgrij ") PREHEAT_1_LABEL _UxGT(" Dizna ~");
    LSTR MSG_PREHEAT_1_ALL                = _UxGT("Predgrij ") PREHEAT_1_LABEL _UxGT(" Sve");
    LSTR MSG_PREHEAT_1_BEDONLY            = _UxGT("Predgrij ") PREHEAT_1_LABEL _UxGT(" Bed");
    LSTR MSG_PREHEAT_1_SETTINGS           = _UxGT("Predgrij ") PREHEAT_1_LABEL _UxGT(" conf");

    LSTR MSG_PREHEAT_M                    = _UxGT("Predgrij $");
    LSTR MSG_PREHEAT_M_H                  = _UxGT("Predgrij $ ~");
    LSTR MSG_PREHEAT_M_END                = _UxGT("Predgrij $ Dizna");
    LSTR MSG_PREHEAT_M_END_E              = _UxGT("Predgrij $ Dizna ~");
    LSTR MSG_PREHEAT_M_ALL                = _UxGT("Predgrij $ Sve");
    LSTR MSG_PREHEAT_M_BEDONLY            = _UxGT("Predgrij $ Bed");
    LSTR MSG_PREHEAT_M_SETTINGS           = _UxGT("Predgrij $ conf");
  #endif
  LSTR MSG_COOLDOWN                       = _UxGT("Hlađenje");
  LSTR MSG_SWITCH_PS_ON                   = _UxGT("Uključi napajanje");
  LSTR MSG_SWITCH_PS_OFF                  = _UxGT("Isključi napajanje");
  LSTR MSG_MOVE_AXIS                      = _UxGT("Miči os");
  LSTR MSG_BED_LEVELING                   = _UxGT("Niveliraj bed");
  LSTR MSG_LEVEL_BED                      = _UxGT("Niveliraj bed");
  LSTR MSG_MOVE_X                         = _UxGT("Miči X");
  LSTR MSG_MOVE_Y                         = _UxGT("Miči Y");
  LSTR MSG_MOVE_N_MM                      = _UxGT("Miči %smm");
  LSTR MSG_MOVE_01MM                      = _UxGT("Miči 0.1mm");
  LSTR MSG_MOVE_1MM                       = _UxGT("Miči 1mm");
  LSTR MSG_MOVE_10MM                      = _UxGT("Miči 10mm");
  LSTR MSG_MOVE_100MM                     = _UxGT("Miči 100mm");
  LSTR MSG_SPEED                          = _UxGT("Brzina");
  LSTR MSG_BED_Z                          = _UxGT("Bed Z");
  LSTR MSG_NOZZLE                         = _UxGT("Dizna");
  LSTR MSG_NOZZLE_N                       = _UxGT("Dizna ~");
  LSTR MSG_BED                            = _UxGT("Bed");
  LSTR MSG_FAN_SPEED                      = _UxGT("Brzina ventilatora");
  LSTR MSG_FAN_SPEED_N                    = _UxGT("Brzina ventilatora ~");
  LSTR MSG_SELECT                         = _UxGT("Odaberi");
  LSTR MSG_SELECT_E                       = _UxGT("Odaberi *");
  LSTR MSG_TEMPERATURE                    = _UxGT("Temperature");
  LSTR MSG_MOTION                         = _UxGT("Gibanje");
  LSTR MSG_VOLUMETRIC_ENABLED             = _UxGT("E in mm") SUPERSCRIPT_THREE;
  LSTR MSG_FILAMENT_DIAM                  = _UxGT("Fil. Dia.");
  LSTR MSG_FILAMENT_DIAM_E                = _UxGT("Fil. Dia. *");
  LSTR MSG_CONTRAST                       = _UxGT("Kontrast LCD-a");
  LSTR MSG_STORE_EEPROM                   = _UxGT("Pohrani u memoriju");
  LSTR MSG_LOAD_EEPROM                    = _UxGT("Učitaj memoriju");
  LSTR MSG_RESTORE_DEFAULTS               = _UxGT("Učitaj Defaults");
  LSTR MSG_REFRESH                        = LCD_STR_REFRESH  _UxGT("Osvježi");
  LSTR MSG_INFO_SCREEN                    = _UxGT("Info screen");
  LSTR MSG_PREPARE                        = _UxGT("Pripremi");
  LSTR MSG_PAUSE_PRINT                    = _UxGT("Pauziraj print");
  LSTR MSG_RESUME_PRINT                   = _UxGT("Nastavi print");
  LSTR MSG_STOP_PRINT                     = _UxGT("Zaustavi print");
  LSTR MSG_MEDIA_MENU                     = _UxGT("CNCaj s SD kartice");
  LSTR MSG_NO_MEDIA                       = _UxGT("Nema SD kartice");
  LSTR MSG_USERWAIT                       = _UxGT("Čekaj korisnika...");
  LSTR MSG_PRINT_ABORTED                  = _UxGT("CNC otkazan");
  LSTR MSG_STOPPED                        = _UxGT("ZAUSTAVLJEN. ");
  LSTR MSG_FILAMENTCHANGE                 = _UxGT("Promijeni filament");
  LSTR MSG_FILAMENTCHANGE_E               = _UxGT("Promijeni filament *");
  LSTR MSG_ATTACH_MEDIA                   = _UxGT("Init. SD karticu");
  LSTR MSG_CHANGE_MEDIA                   = _UxGT("Promijeni SD karticu");
  LSTR MSG_HEATING_FAILED_LCD             = _UxGT("Grijanje neuspješno");
  LSTR MSG_HEATING                        = _UxGT("Grijanje...");
  LSTR MSG_BED_HEATING                    = _UxGT("Grijanje Bed-a...");
  LSTR MSG_DELTA_CALIBRATE                = _UxGT("Delta Kalibracija");
  LSTR MSG_DELTA_CALIBRATE_X              = _UxGT("Kalibriraj X");
  LSTR MSG_DELTA_CALIBRATE_Y              = _UxGT("Kalibriraj Y");
  LSTR MSG_DELTA_CALIBRATE_Z              = _UxGT("Kalibriraj Z");
  LSTR MSG_DELTA_CALIBRATE_CENTER         = _UxGT("Kalibriraj Središte");
  LSTR MSG_INFO_MENU                      = _UxGT("O cncu");
  LSTR MSG_INFO_PRINTER_MENU              = _UxGT("Podaci o cncu");
  LSTR MSG_INFO_STATS_MENU                = _UxGT("Statistika cnca");
  LSTR MSG_INFO_BOARD_MENU                = _UxGT("Podaci o elektronici");
  LSTR MSG_INFO_THERMISTOR_MENU           = _UxGT("Termistori");
  LSTR MSG_INFO_ATC_TOOLS                 = _UxGT("Extruderi");
  LSTR MSG_INFO_BAUDRATE                  = _UxGT("Baud");
  LSTR MSG_INFO_PROTOCOL                  = _UxGT("Protokol");
  LSTR MSG_CASE_LIGHT                     = _UxGT("Osvjetljenje");

  LSTR MSG_KILL_EXPECTED_PRINTER          = _UxGT("Neispravan pisač");

  #if LCD_WIDTH >= 20 || HAS_DWIN_E3V2
    LSTR MSG_INFO_PRINT_COUNT             = _UxGT("Broj printova");
    LSTR MSG_INFO_COMPLETED_PRINTS        = _UxGT("Završeni");
    LSTR MSG_INFO_PRINT_TIME              = _UxGT("Ukupno printanja");
    LSTR MSG_INFO_PRINT_LONGEST           = _UxGT("Najduži print");
    LSTR MSG_INFO_PRINT_FILAMENT          = _UxGT("Extrudirano ukupno");
  #else
    LSTR MSG_INFO_PRINT_COUNT             = _UxGT("CNCovi");
    LSTR MSG_INFO_COMPLETED_PRINTS        = _UxGT("Završeni");
    LSTR MSG_INFO_PRINT_TIME              = _UxGT("Ukupno");
    LSTR MSG_INFO_PRINT_LONGEST           = _UxGT("Najduži");
    LSTR MSG_INFO_PRINT_FILAMENT          = _UxGT("Extrudirano");
  #endif

  LSTR MSG_INFO_PSU                       = _UxGT("Napajanje");

  LSTR MSG_FILAMENT_CHANGE_OPTION_RESUME  = _UxGT("Nastavi print");

  LSTR MSG_FILAMENT_CHANGE_UNLOAD         = _UxGT(MSG_2_LINE("Čekaj", "filament unload"));
  LSTR MSG_FILAMENT_CHANGE_LOAD           = _UxGT(MSG_2_LINE("Pričekaj", "filament load"));
  LSTR MSG_FILAMENT_CHANGE_RESUME         = _UxGT(MSG_1_LINE("Nastavljam..."));

  #if LCD_HEIGHT >= 4
    // Up to 3 lines allowed
    LSTR MSG_FILAMENT_CHANGE_INIT         = _UxGT(MSG_3_LINE("Čekaj početak", "filamenta", "promijeni"));
    LSTR MSG_FILAMENT_CHANGE_INSERT       = _UxGT(MSG_3_LINE("Umetni filament", "i pritisni tipku", "za nastavak..."));
  #else
    // Up to 2 lines allowed
    LSTR MSG_FILAMENT_CHANGE_INIT         = _UxGT("Pričekaj...");
  #endif
}
