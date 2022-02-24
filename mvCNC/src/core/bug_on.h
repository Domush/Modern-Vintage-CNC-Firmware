/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

// We need SERIAL_ECHOPGM and macros.h
#include "serial.h"

#if ENABLED(POSTMORTEM_DEBUGGING)
  // Useful macro for stopping the CPU on an unexpected condition
  // This is used like SERIAL_ECHOPGM, that is: a key-value call of the local variables you want
  // to dump to the serial port before stopping the CPU.
                          // \/ Don't replace by SERIAL_ECHOPGM since ONLY_FILENAME cannot be transformed to a PGM string on Arduino and it breaks building
  #define BUG_ON(V...) do { SERIAL_ECHO(ONLY_FILENAME); SERIAL_ECHO(__LINE__); SERIAL_ECHOLNPGM(": "); SERIAL_ECHOLNPGM(V); SERIAL_FLUSHTX(); *(char*)0 = 42; } while(0)
#elif ENABLED(mvCNC_DEV_MODE)
  // Don't stop the CPU here, but at least dump the bug on the serial port
                          // \/ Don't replace by SERIAL_ECHOPGM since ONLY_FILENAME cannot be transformed to a PGM string on Arduino and it breaks building
  #define BUG_ON(V...) do { SERIAL_ECHO(ONLY_FILENAME); SERIAL_ECHO(__LINE__); SERIAL_ECHOLNPGM(": BUG!"); SERIAL_ECHOLNPGM(V); SERIAL_FLUSHTX(); } while(0)
#else
  // Release mode, let's ignore the bug
  #define BUG_ON(V...) NOOP
#endif
