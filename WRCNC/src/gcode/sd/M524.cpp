/**
 * Webber Ranch CNC Firmware
*/

#include "../../inc/WRCNCConfig.h"

#if ENABLED(SDSUPPORT)

#include "../gcode.h"
#include "../../sd/cardreader.h"

/**
 * M524: Abort the current SD print job (started with M24)
 */
void GcodeSuite::M524() {

  if (IS_SD_PRINTING())
    card.abortFilePrintSoon();
  else if (card.isMounted())
    card.closefile();

}

#endif // SDSUPPORT
