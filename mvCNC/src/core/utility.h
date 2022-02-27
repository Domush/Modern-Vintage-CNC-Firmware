/**
 * Modern Vintage CNC Firmware
*/
#pragma once

#include "../inc/mvCNCConfigPre.h"
#include "../core/types.h"
#include "../core/millis_t.h"

void safe_delay(millis_t ms);           // Delay ensuring that temperatures are updated and the watchdog is kept alive.

#if ENABLED(SERIAL_OVERRUN_PROTECTION)
  void serial_delay(const millis_t ms);
#else
  inline void serial_delay(const millis_t) {}
#endif

#if (GRID_MAX_POINTS_X) && (GRID_MAX_POINTS_Y)

  // 16x16 bit arrays
  template <int W, int H>
  struct FlagBits {
    typename IF<(W>8), uint16_t, uint8_t>::type bits[H];
    void fill()                                   { memset(bits, 0xFF, sizeof(bits)); }
    void reset()                                  { memset(bits, 0x00, sizeof(bits)); }
    void unmark(const uint8_t x, const uint8_t y) { CBI(bits[y], x); }
    void mark(const uint8_t x, const uint8_t y)   { SBI(bits[y], x); }
    bool marked(const uint8_t x, const uint8_t y) { return TEST(bits[y], x); }
    inline void unmark(const xy_int8_t &xy)       { unmark(xy.x, xy.y); }
    inline void mark(const xy_int8_t &xy)         { mark(xy.x, xy.y); }
    inline bool marked(const xy_int8_t &xy)       { return marked(xy.x, xy.y); }
  };

  typedef FlagBits<GRID_MAX_POINTS_X, GRID_MAX_POINTS_Y> MeshFlags;

#endif

  #define log_machine_info() NOOP

template<typename T>
class restorer {
  T& ref_;
  T  val_;
public:
  restorer(T& perm) : ref_(perm), val_(perm) {}
  restorer(T& perm, T temp_val) : ref_(perm), val_(perm) { perm = temp_val; }
  ~restorer() { restore(); }
  inline void restore() { ref_ = val_; }
};

#define REMEMBER(N,X,V...) restorer<__typeof__(X)> restorer_##N(X, ##V)
#define RESTORE(N) restorer_##N.restore()

// Converts from an uint8_t in the range of 0-255 to an uint8_t
// in the range 0-100 while avoiding rounding artifacts
constexpr uint8_t ui8_to_percent(const uint8_t i) { return (int(i) * 100 + 127) / 255; }

const xyze_char_t axis_codes LOGICAL_AXIS_ARRAY('E', 'X', 'Y', 'Z', AXIS4_NAME, AXIS5_NAME, AXIS6_NAME);

#if LINEAR_AXES <= XYZ
  #define AXIS_CHAR(A) ((char)('X' + A))
#else
  #define AXIS_CHAR(A) axis_codes[A]
#endif
