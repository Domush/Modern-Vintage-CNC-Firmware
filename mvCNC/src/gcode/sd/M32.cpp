/**
 * Modern Vintage CNC Firmware
*/

#include "../../inc/mvCNCConfig.h"

#if HAS_MEDIA_SUBCALLS

#include "../gcode.h"
#include "../../sd/cardreader.h"
#include "../../module/planner.h" // for synchronize()

#include "../../mvCNCCore.h" // for startOrResumeJob

/**
 * M32: Select file and start SD Print
 *
 * Examples:
 *
 *    M32 !PATH/TO/FILE.GCO#      ; Start FILE.GCO
 *    M32 P !PATH/TO/FILE.GCO#    ; Start FILE.GCO as a procedure
 *    M32 S60 !PATH/TO/FILE.GCO#  ; Start FILE.GCO at byte 60
 */
void GcodeSuite::M32() {
  if (IS_SD_JOB_RUNNING()) planner.synchronize();

  if (card.isMounted()) {
    const uint8_t call_procedure = parser.boolval('P');

    card.openFileRead(parser.string_arg, call_procedure);

    if (parser.seenval('S')) card.setIndex(parser.value_long());

    card.startOrResumeFileCutting();

    // Procedure calls count as normal print time.
    if (!call_procedure) startOrResumeJob();
  }
}

#endif // HAS_MEDIA_SUBCALLS
