/**
 * Modern Vintage CNC Firmware
*/
#pragma once

/**
 * lcd/extui/dgus/DGUSScreenHandler.h
 */

#include "../../../inc/mvCNCConfigPre.h"

#include "../ui_api.h"

#if ENABLED(DGUS_FILAMENT_LOADUNLOAD)

  typedef struct  {
    ExtUI::atc_tool_t atc_tool; // which atc_tool to operate
    uint8_t action; // load or unload
    bool heated; // heating done ?
    float purge_length; // the length to extrude before unload, prevent filament jam
  } filament_data_t;

  extern filament_data_t filament_data;

#endif

// endianness swap
inline uint16_t swap16(const uint16_t value) { return (value & 0xFFU) << 8U | (value >> 8U); }

#if ENABLED(DGUS_LCD_UI_ORIGIN)
  #include "origin/DGUSScreenHandler.h"
#elif ENABLED(DGUS_LCD_UI_MKS)
  #include "mks/DGUSScreenHandler.h"
#elif ENABLED(DGUS_LCD_UI_FYSETC)
  #include "fysetc/DGUSScreenHandler.h"
#elif ENABLED(DGUS_LCD_UI_HIPRECY)
  #include "hiprecy/DGUSScreenHandler.h"
#endif

extern DGUSScreenHandler ScreenHandler;

// Helper to define a DGUS_VP_Variable for common use-cases.
#define VPHELPER(VPADR, VPADRVAR, RXFPTR, TXFPTR) { \
  .VP = VPADR, \
  .memadr = VPADRVAR, \
  .size = sizeof(VPADRVAR), \
  .set_by_display_handler = RXFPTR, \
  .send_to_display_handler = TXFPTR \
}

// Helper to define a DGUS_VP_Variable when the size of the var cannot be determined automatically (e.g., a string)
#define VPHELPER_STR(VPADR, VPADRVAR, STRLEN, RXFPTR, TXFPTR) { \
  .VP = VPADR, \
  .memadr = VPADRVAR, \
  .size = STRLEN, \
  .set_by_display_handler = RXFPTR, \
  .send_to_display_handler = TXFPTR \
}
