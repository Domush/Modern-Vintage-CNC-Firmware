/**
 * Modern Vintage CNC Firmware
*/
#pragma once

#include "../inc/mvCNCConfig.h"

/**
 * @brief Nozzle class
 *
 * @todo: Do not ignore the end.z value and allow XYZ movements
 */
class Nozzle {
  private:

  #if ENABLED(NOZZLE_CLEAN_FEATURE)

    /**
     * @brief Stroke clean pattern
     * @details Wipes the nozzle back and forth in a linear movement
     *
     * @param start xyz_pos_t defining the starting point
     * @param end xyz_pos_t defining the ending point
     * @param strokes number of strokes to execute
     */
    static void stroke(const xyz_pos_t &start, const xyz_pos_t &end, const uint8_t &strokes) _Os;

    /**
     * @brief Zig-zag clean pattern
     * @details Apply a zig-zag cleaning pattern
     *
     * @param start xyz_pos_t defining the starting point
     * @param end xyz_pos_t defining the ending point
     * @param strokes number of strokes to execute
     * @param objects number of objects to create
     */
    static void zigzag(const xyz_pos_t &start, const xyz_pos_t &end, const uint8_t &strokes, const uint8_t &objects) _Os;

    /**
     * @brief Circular clean pattern
     * @details Apply a circular cleaning pattern
     *
     * @param start xyz_pos_t defining the middle of circle
     * @param strokes number of strokes to execute
     * @param radius radius of circle
     */
    static void circle(const xyz_pos_t &start, const xyz_pos_t &middle, const uint8_t &strokes, const_float_t radius) _Os;

  #endif // NOZZLE_CLEAN_FEATURE

  public:

  #if ENABLED(NOZZLE_CLEAN_FEATURE)

    /**
     * @brief Clean the nozzle
     * @details Starts the selected clean procedure pattern
     *
     * @param pattern one of the available patterns
     * @param argument depends on the cleaning pattern
     */
    static void clean(const uint8_t &pattern, const uint8_t &strokes, const_float_t radius, const uint8_t &objects, const uint8_t cleans) _Os;

  #endif // NOZZLE_CLEAN_FEATURE

  #if ENABLED(SPINDLE_PARK_FEATURE)

    static float park_mode_0_height(const_float_t park_z) _Os;
    static void park(const uint8_t z_action, const xyz_pos_t &park=SPINDLE_PARK_POINT) _Os;

  #endif
};

extern Nozzle nozzle;
