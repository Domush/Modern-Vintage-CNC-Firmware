/**
 * Modern Vintage CNC Firmware
*/
#pragma once

#include "../../../inc/mvCNCConfigPre.h"
#if ENABLED(EMERGENCY_PARSER)
  #include "../../../feature/e_parser.h"
#endif
#include "../../../core/serial_hook.h"

#include <stdarg.h>
#include <stdio.h>

/**
 * Generic RingBuffer
 * T type of the buffer array
 * S size of the buffer (must be power of 2)
 */
template <typename T, uint32_t S> class RingBuffer {
public:
  RingBuffer() { index_read = index_write = 0; }
  uint32_t available() volatile { return index_write - index_read; }
  uint32_t free() volatile      { return buffer_size - available(); }
  bool empty() volatile         { return index_read == index_write; }
  bool full() volatile          { return available() == buffer_size; }
  void clear() volatile         { index_read = index_write = 0; }

  bool peek(T *value) volatile {
    if (value == 0 || available() == 0)
      return false;
    *value = buffer[mask(index_read)];
    return true;
  }

  int read() volatile {
    if (empty()) return -1;
    return buffer[mask(index_read++)];
  }

  bool write(T value) volatile {
    if (full()) return false;
    buffer[mask(index_write++)] = value;
    return true;
  }

private:
  uint32_t mask(uint32_t val) volatile {
    return buffer_mask & val;
  }

  static const uint32_t buffer_size = S;
  static const uint32_t buffer_mask = buffer_size - 1;
  volatile T buffer[buffer_size];
  volatile uint32_t index_write;
  volatile uint32_t index_read;
};

struct HalSerial {
  HalSerial() { host_connected = true; }

  void begin(int32_t) {}
  void end()          {}

  int peek() {
    uint8_t value;
    return receive_buffer.peek(&value) ? value : -1;
  }

  int read() { return receive_buffer.read(); }

  size_t write(char c) {
    if (!host_connected) return 0;
    while (!transmit_buffer.free());
    return transmit_buffer.write(c);
  }

  bool connected() { return host_connected; }

  uint16_t available() {
    return (uint16_t)receive_buffer.available();
  }

  void flush() { receive_buffer.clear(); }

  uint8_t availableForWrite() {
    return transmit_buffer.free() > 255 ? 255 : (uint8_t)transmit_buffer.free();
  }

  void flushTX() {
    if (host_connected)
      while (transmit_buffer.available()) { /* nada */ }
  }

  volatile RingBuffer<uint8_t, 128> receive_buffer;
  volatile RingBuffer<uint8_t, 128> transmit_buffer;
  volatile bool host_connected;
};

typedef Serial1Class<HalSerial> MSerialT;
