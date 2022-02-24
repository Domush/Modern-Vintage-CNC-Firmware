/**
 * Modern Vintage CNC Firmware
*/
#pragma once

#include "../core/macros.h"

#include <Wire.h>

// Print debug messages with M111 S2 (Uses 236 bytes of PROGMEM)
//#define DEBUG_TWIBUS

typedef void (*twiReceiveFunc_t)(int bytes);
typedef void (*twiRequestFunc_t)();

/**
 * For a light i2c protocol that runs on two boards running mvCNC see:
 * See https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware/issues/4776#issuecomment-246262879
 */
#if I2C_SLAVE_ADDRESS > 0

  void i2c_on_receive(int bytes); // Demo i2c onReceive handler
  void i2c_on_request();          // Demo i2c onRequest handler

#endif

#define TWIBUS_BUFFER_SIZE 32

/**
 * TWIBUS class
 *
 * This class implements a wrapper around the two wire (I2C) bus, allowing
 * mvCNC to send and request data from any slave device on the bus.
 *
 * The two main consumers of this class are M260 and M261. M260 provides a way
 * to send an I2C packet to a device (no repeated starts) by caching up to 32
 * bytes in a buffer and then sending the buffer.
 * M261 requests data from a device. The received data is relayed to serial out
 * for the host to interpret.
 *
 *  For more information see
 *    - https://mvcncfw.org/docs/gcode/M260.html
 *    - https://mvcncfw.org/docs/gcode/M261.html
 */
class TWIBus {
  private:
    /**
     * @brief Number of bytes on buffer
     * @description Number of bytes in the buffer waiting to be flushed to the bus
     */
    uint8_t buffer_s = 0;

    /**
     * @brief Internal buffer
     * @details A fixed buffer. TWI commands can be no longer than this.
     */
    uint8_t buffer[TWIBUS_BUFFER_SIZE];


  public:
    /**
     * @brief Target device address
     * @description The target device address. Persists until changed.
     */
    uint8_t addr = 0;

    /**
     * @brief Class constructor
     * @details Initialize the TWI bus and clear the buffer
     */
    TWIBus();

    /**
     * @brief Reset the buffer
     * @details Set the buffer to a known-empty state
     */
    void reset();

    /**
     * @brief Send the buffer data to the bus
     * @details Flush the buffer to the target address
     */
    void send();

    /**
     * @brief Add one byte to the buffer
     * @details Add a byte to the end of the buffer.
     *          Silently fails if the buffer is full.
     *
     * @param c a data byte
     */
    void addbyte(const char c);

    /**
     * @brief Add some bytes to the buffer
     * @details Add bytes to the end of the buffer.
     *          Concatenates at the buffer size.
     *
     * @param src source data address
     * @param bytes the number of bytes to add
     */
    void addbytes(char src[], uint8_t bytes);

    /**
     * @brief Add a null-terminated string to the buffer
     * @details Add bytes to the end of the buffer up to a nul.
     *          Concatenates at the buffer size.
     *
     * @param str source string address
     */
    void addstring(char str[]);

    /**
     * @brief Set the target slave address
     * @details The target slave address for sending the full packet
     *
     * @param adr 7-bit integer address
     */
    void address(const uint8_t adr);

    /**
     * @brief Prefix for echo to serial
     * @details Echo a label, length, address, and "data:"
     *
     * @param bytes the number of bytes to request
     */
    static void echoprefix(uint8_t bytes, FSTR_P const prefix, uint8_t adr);

    /**
     * @brief Echo data on the bus to serial
     * @details Echo some number of bytes from the bus
     *          to serial in a parser-friendly format.
     *
     * @param bytes the number of bytes to request
     * @param style Output format for the bytes, 0 = Raw byte [default], 1 = Hex characters, 2 = uint16_t
     */
    static void echodata(uint8_t bytes, FSTR_P const prefix, uint8_t adr, const uint8_t style=0);

    /**
     * @brief Echo data in the buffer to serial
     * @details Echo the entire buffer to serial
     *          to serial in a parser-friendly format.
     *
     * @param bytes the number of bytes to request
     */
    void echobuffer(FSTR_P const prefix, uint8_t adr);

    /**
     * @brief Request data from the slave device and wait.
     * @details Request a number of bytes from a slave device.
     *          Wait for the data to arrive, and return true
     *          on success.
     *
     * @param bytes the number of bytes to request
     * @return status of the request: true=success, false=fail
     */
    bool request(const uint8_t bytes);

    /**
     * @brief Capture data from the bus into the buffer.
     * @details Capture data after a request has succeeded.
     *
     * @param bytes the number of bytes to request
     * @return the number of bytes captured to the buffer
     */
    uint8_t capture(char *dst, const uint8_t bytes);

    /**
     * @brief Flush the i2c bus.
     * @details Get all bytes on the bus and throw them away.
     */
    static void flush();

    /**
     * @brief Request data from the slave device, echo to serial.
     * @details Request a number of bytes from a slave device and output
     *          the returned data to serial in a parser-friendly format.
     * @style Output format for the bytes, 0 = raw byte [default], 1 = Hex characters, 2 = uint16_t
     *
     * @param bytes the number of bytes to request
     */
    void relay(const uint8_t bytes, const uint8_t style=0);

    #if I2C_SLAVE_ADDRESS > 0

      /**
       * @brief Register a slave receive handler
       * @details Set a handler to receive data addressed to us
       *
       * @param handler A function to handle receiving bytes
       */
      inline void onReceive(const twiReceiveFunc_t handler) { Wire.onReceive(handler); }

      /**
       * @brief Register a slave request handler
       * @details Set a handler to send data requested from us
       *
       * @param handler A function to handle receiving bytes
       */
      inline void onRequest(const twiRequestFunc_t handler) { Wire.onRequest(handler); }

      /**
       * @brief Default handler to receive
       * @details Receive bytes sent to our slave address
       *          and simply echo them to serial.
       */
      void receive(uint8_t bytes);

      /**
       * @brief Send a reply to the bus
       * @details Send the buffer and clear it.
       *          If a string is passed, write it into the buffer first.
       */
      void reply(char str[]=nullptr);
      inline void reply(const char str[]) { reply((char*)str); }

    #endif

    #if ENABLED(DEBUG_TWIBUS)
      /**
       * @brief Prints a debug message
       * @details Prints a simple debug message "TWIBus::function: value"
       */
      static void prefix(FSTR_P const func);
      static void debug(FSTR_P const func, uint32_t adr);
      static void debug(FSTR_P const func, char c);
      static void debug(FSTR_P const func, char adr[]);
    #else
      static void debug(FSTR_P const, uint32_t) {}
      static void debug(FSTR_P const, char) {}
      static void debug(FSTR_P const, char[]) {}
    #endif
    static void debug(FSTR_P const func, uint8_t v) { debug(func, (uint32_t)v); }
};

extern TWIBus i2c;
