/**
 * Modern Vintage CNC Firmware
*/
#pragma once

#include <HardwareSerial.h>
#include <WString.h>

#include "../../inc/mvCNCConfigPre.h"
#if ENABLED(EMERGENCY_PARSER)
  #include "../../feature/e_parser.h"
#endif
#include "../../core/serial_hook.h"

#ifndef SERIAL_PORT
  #define SERIAL_PORT 0
#endif
#ifndef RX_BUFFER_SIZE
  #define RX_BUFFER_SIZE 128
#endif
#ifndef TX_BUFFER_SIZE
  #define TX_BUFFER_SIZE 32
#endif

class mvCNCSerial : public HardwareSerial<RX_BUFFER_SIZE, TX_BUFFER_SIZE> {
public:
  mvCNCSerial(LPC_UART_TypeDef *UARTx) : HardwareSerial<RX_BUFFER_SIZE, TX_BUFFER_SIZE>(UARTx) { }

  void end() {}

  int availableForWrite(void) { /* flushTX(); */ return TX_BUFFER_SIZE; }

  #if ENABLED(EMERGENCY_PARSER)
    bool recv_callback(const char c) override;
  #endif
};

// On LPC176x framework, HardwareSerial does not implement the same interface as Arduino's Serial, so overloads
// of 'available' and 'read' method are not used in this multiple inheritance scenario.
// Instead, use a ForwardSerial here that adapts the interface.
typedef ForwardSerial1Class<mvCNCSerial> MSerialT;
extern MSerialT MSerial0;
extern MSerialT MSerial1;
extern MSerialT MSerial2;
extern MSerialT MSerial3;

// Consequently, we can't use a RuntimeSerial either. The workaround would be to use
// a RuntimeSerial<ForwardSerial<mvCNCSerial>> type here. Ignore for now until it's actually required.
#if ENABLED(SERIAL_RUNTIME_HOOK)
  #error "SERIAL_RUNTIME_HOOK is not yet supported for LPC176x."
#endif
