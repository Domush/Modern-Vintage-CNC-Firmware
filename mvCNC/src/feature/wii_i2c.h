/**
 * Modern Vintage CNC Firmware
*/
#pragma once

/**
 * Wii Nunchuck input / jogging
 */

#include "../inc/mvCNCConfigPre.h"
#include "../core/types.h"

#define NUNCHUK_BUFFER_SIZE 6
#ifndef WII_SLOW_DIVISER
  #define WII_SLOW_DIVISER 3 // Divide all speeds by value for standard (C unpressed) movements
#endif
#ifndef WII_X_LIMITS
  #define WII_X_LIMITS { 0, 128-10, 128+10, 255 } // min, deadzone start, deadzone end, max
#endif
#ifndef WII_Y_LIMITS
  #define WII_Y_LIMITS { 0, 128-10, 128+10, 255 }
#endif

class WiiNunchuck {
private:
  #if ENABLED(WII_NUNCHUCK_DEBUG)
    void report();
  #endif
  void calculate(xyz_float_t &joy_value_normalized);
	void requestData();
	char decodeByte(const char);

	unsigned char _buffer[NUNCHUK_BUFFER_SIZE];

public:
	void connect();
	bool update();
  void injectJogMoves();

	uint8_t joyX() const { return _buffer[0];}
	uint8_t joyY() const { return _buffer[1];}

	bool left() const { return _buffer[0] <50;}
	bool right() const { return _buffer[0] > 200;}
	bool up() const {return _buffer[1] > 200;}
	bool down() const  {return _buffer[1] < 60;}

	uint16_t xAcceleration() const {
		return ((uint16_t) (_buffer[2] << 2) | ((_buffer[5] >> 2) & 0x03));
	}
	uint16_t yAcceleration() const {
		return ((uint16_t) (_buffer[3] << 2) | ((_buffer[5] >> 4) & 0x03));
	}
	uint16_t zAcceleration() const {
		return ((uint16_t) (_buffer[4] <<2 ) | ((_buffer[5] >> 6) & 0x03));
	}

	bool zPressed() const {return (_buffer[5] & 0x01);}
	bool cPressed() const {return ((_buffer[5] >> 1) & 0x01); }
};

// class WiiNunchuck {
// #if ENABLED(WII_NUNCHUCK_DEBUG)
//   static void report();
// #endif
//   static void calculate(xyz_float_t &norm_jog);

// public:
//   void inject_jog_moves();

//   static Nunchuk state;
//   void connect();
//   void update();
// };

extern WiiNunchuck wii;
