/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#include "../../../../inc/mvCNCConfigPre.h"

#if ENABLED(DGUS_LCD_UI_RELOADED)

#include "DGUS_ScreenSetup.h"

#include "../DGUSSetupHandler.h"

#include "../../ui_api.h"

#define SETUP_HELPER(SCREEN, SETUP) \
  { .screen = SCREEN, \
  .setup_fn = SETUP }

const struct DGUS_ScreenSetup screen_setup_list[] PROGMEM = {
  #if ENABLED(SDSUPPORT)
    SETUP_HELPER(DGUS_Screen::PRINT,            &DGUSSetupHandler::CNC),
  #endif
  SETUP_HELPER(DGUS_Screen::PRINT_STATUS,       &DGUSSetupHandler::CNCStatus),
  SETUP_HELPER(DGUS_Screen::PRINT_ADJUST,       &DGUSSetupHandler::CNCAdjust),
  SETUP_HELPER(DGUS_Screen::LEVELING_MENU,      &DGUSSetupHandler::LevelingMenu),
  SETUP_HELPER(DGUS_Screen::LEVELING_OFFSET,    &DGUSSetupHandler::LevelingOffset),
  SETUP_HELPER(DGUS_Screen::LEVELING_MANUAL,    &DGUSSetupHandler::LevelingManual),
  SETUP_HELPER(DGUS_Screen::LEVELING_AUTOMATIC, &DGUSSetupHandler::LevelingAutomatic),
  SETUP_HELPER(DGUS_Screen::LEVELING_PROBING,   &DGUSSetupHandler::LevelingProbing),
  SETUP_HELPER(DGUS_Screen::FILAMENT,           &DGUSSetupHandler::Filament),
  SETUP_HELPER(DGUS_Screen::MOVE,               &DGUSSetupHandler::Move),
  SETUP_HELPER(DGUS_Screen::GCODE,              &DGUSSetupHandler::Gcode),
  SETUP_HELPER(DGUS_Screen::PID,                &DGUSSetupHandler::PID),
  SETUP_HELPER(DGUS_Screen::INFOS,              &DGUSSetupHandler::Infos),

  SETUP_HELPER((DGUS_Screen)0, nullptr)
};

#endif // DGUS_LCD_UI_RELOADED
